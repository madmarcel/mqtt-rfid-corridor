const FOLDER = 'sound/';

var sounds = {
    "successread" : {
      url : "activate02.wav"
    },
    "door" : {
      url : "df_door1-1.wav",
    },
    "bloop" : {
      url : "nrgpu1.wav"
    },
    "click": {
        url : "lgclick1.wav"
    },
    "ignoreread" : {
        url : "activate04.wav"
    },
    "nuh-uh": {
        url: "scalarm.wav"
    },
    "alarm": {
        url: "00alarmloop01.wav"
    }
  };
  
  var soundContext = new AudioContext();
  
  for(var key in sounds) {
    loadSound(key);
  }
  
  function loadSound(name){
    var sound = sounds[name];
  
    var url = FOLDER + sound.url;
    var buffer = sound.buffer;
  
    var request = new XMLHttpRequest();
    request.open('GET', url, true);
    request.responseType = 'arraybuffer';
  
    request.onload = function() {
      soundContext.decodeAudioData(request.response, function(newBuffer) {
        sound.buffer = newBuffer;
      });
    }
  
    request.send();
  }
  
  function playSound(name, options, endCallback){
    var sound = sounds[name];
    var soundVolume = sounds[name].volume || 1;
  
    var buffer = sound.buffer;
    if(buffer){
      var source = soundContext.createBufferSource();
      source.buffer = buffer;
  
      var volume = soundContext.createGain();
  
      if(options) {
        if(options.volume) {
          volume.gain.value = soundVolume * options.volume;
        }
      } else {
        volume.gain.value = soundVolume;
      }
  
      volume.connect(soundContext.destination);
      source.connect(volume);
      if(endCallback) {
        source.addEventListener('ended', endCallback, false);
      }
      source.start(0);
    }
  }