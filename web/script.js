var statusElement = document.getElementById('status');
var progressElement = document.getElementById('progress');
var spinnerElement = document.getElementById('spinner');

var canvas = document.getElementById('canvas');
canvas.width = window.innerWidth;
canvas.height = window.innerHeight;

// Issue: https://github.com/emscripten-core/emscripten/issues/11221
// 
// This is a hack to fix the issue that the Emscripten GLFW imlementation
// does not allow backspace.
let preventEvents = false;
function stopImmediatePropagation(event) {
  if (preventEvents) {
    event.stopImmediatePropagation()
  }
}
window.addEventListener('keydown', (event) => {
  stopImmediatePropagation(event)
  if (preventEvents) {
    if (event.keyCode === 13) {
      // console.log(event.key);
      swal.clickConfirm()
    }
  }
  // Prevent default browser behaviour
  if(event.ctrlKey && (event.which == 83)) {
    event.preventDefault();
    return false;
  }
}, true);
window.addEventListener('keyup', stopImmediatePropagation, true);

var Module = {
  preRun: [],
  postRun: [],
  print: (function() {
    return function(text) {
      if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
      console.log(text);
    };
  })(),
  canvas: (function() {
    window.addEventListener("resize", function(e) {
      canvas.width = window.innerWidth;
      canvas.height = window.innerHeight;
      window.Module._gate_resizeWindow(window.innerWidth, window.innerHeight);
    })
    // As a default initial behavior, pop up an alert when webgl context is lost. To make your
    // application robust, you may want to override this behavior before shipping!
    // See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
    canvas.addEventListener("webglcontextlost", function(e) { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false);

    return canvas;
  })(),
  saveFile: async function(content) {
    preventEvents = true;
    const { value: name } = await Swal.fire({
      title: 'Save with file name?',
      input: 'text',
      inputAttributes: {
        required: true,
        maxlength: 255,
        autocapitalize: 'off',
        autocorrect: 'off'
      },
      inputPlaceholder: 'MyAwesomeCircuit',
      inputAutoTrim: true,
      inputValidator: (value) => {
        if (!value) {
          return 'You need to write the name for the save file!'
        }
      },
      allowEnterKey: false,
      showCancelButton: true,
      confirmButtonText: 'Save',
    });

    preventEvents = false;
    if (name) {
      var file = new File(
        [content],
        `${name}.json`,
        { type: "text/plain;charset=utf-8"}
      )
      saveAs(file);

      await Swal.fire({
        title: `File ${name}.json saved!`,
      });
    }
  },
  // loadFile: function(name, content) {
  //   window.Module._gate_loadFile();
  // },
  setStatus: function(text) {
    if (!Module.setStatus.last) Module.setStatus.last = { time: Date.now(), text: '' };
    if (text === Module.setStatus.last.text) return;
    var m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
    var now = Date.now();
    if (m && now - Module.setStatus.last.time < 30) return; // if this is a progress update, skip it if too soon
    Module.setStatus.last.time = now;
    Module.setStatus.last.text = text;
    if (m) {
      text = m[1];
      progressElement.value = parseInt(m[2])*100;
      progressElement.max = parseInt(m[4])*100;
      progressElement.hidden = false;
      spinnerElement.hidden = false;
    } else {
      progressElement.value = null;
      progressElement.max = null;
      progressElement.hidden = true;
      if (!text) spinnerElement.style.display = 'none';
    }
    statusElement.innerHTML = text;
  },
  totalDependencies: 0,
  monitorRunDependencies: function(left) {
    this.totalDependencies = Math.max(this.totalDependencies, left);
    Module.setStatus(left ? 'Preparing... (' + (this.totalDependencies-left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
  }
};

Module.setStatus('Downloading...');
window.onerror = function(event) {
  // TODO: do not warn on ok events like simulating an infinite loop or exitStatus
  Module.setStatus('Exception thrown, see JavaScript console');
  spinnerElement.style.display = 'none';
  Module.setStatus = function(text) {
    if (text) Module.printErr('[post-exception status] ' + text);
  };
};