'use strict';

window.addEventListener('load', () => {
    
    /* ---------------------------------- */

    // these have to be lowercase
    const GOODTAGS = [
        '04ba56129c368000',
        '04a856129c368000',
        
        '04382c4a312c8000',
        '044d3052312c8000',

        '047a56129c368000',
        '04c456129c368000'

    ];

    const CLOSE_SIGNAL = 'closedoor';

    /* ---------------------------------- */

    // mqtt client
    const client  = mqtt.connect('ws://localhost:8083');

    client.on('connect', (e) => {
        console.log('Connected to MQTT server', e);
        client.subscribe('rfid');
    });
    
    // process mqtt events
    client.on('message', (topic, payload) => {
        let t = topic.toLowerCase();
        let p = payload.toString().toLowerCase();
        console.log('RFID Read', t, p);

        processTag(p);
    });

    // check the received tag
    let processTag = (tag) => {
        if(GOODTAGS.indexOf(tag) > -1) {
            openDoorEvent();
        } else if(tag === CLOSE_SIGNAL) {
            closeDoorEvent();
        } else {
            badReadEvent();
        }
    }

    /* ---------------------------------- */

    let ignore = false;
    
    const STATES = {
        'CLOSED': 0,
        'OPENING': 1,
        'OPEN': 2,
        'CLOSING': 3,
        'BADREAD': 4
    };

    let currentState = STATES.CLOSED;
    let nextState = null;

    let openDoorEvent = () => {
        if(isClosed()) {
            playSound('successread', { volume: 1.0 });
            ignore = true;
            currentState = STATES.OPENING;
            setTimeout(openDoors, 400);
        } else {
            playSound('ignoreread', { volume: 1.0 });
        }
    }

    let closeDoorEvent = () => {
        if(isOpen()) {
            playSound('bloop', { volume: 1.0 });
            ignore = true;
            currentState = STATES.CLOSING;
            setTimeout(closeDoors, 400);
        } else {
            playSound('click', { volume: 1.0 });
        }
    }

    let badReadEvent = () => {
        if(!isAlarm()) {
            playSound('nuh-uh', { volume: 1.0 });
            if(isOpen()) {
                setTimeout(closeDoors, 400);
            }
            ignore = true;
            currentState = STATES.BADREAD;
            startAlarm();
        } else {
            playSound('nuh-uh', { volume: 1.0 });
        }
    }

    /* ---------------------------------- */

    let compareState = (state) => {
        return currentState === STATES[state];
    }

    let isOpen = () => {
        return compareState('OPEN');
    }

    let isClosed = () => {
        return compareState('CLOSED');
    }

    let isAlarm = () => {
        return compareState('BADREAD');
    }

    /* ---------------------------------- */

    let startRedLights = () => {
        document.querySelector('#redlight').emit('start');
    }

    let stopRedLights = () => {
        document.querySelector('#redlight').emit('stop');
    }

    let startAlarm = () => {
        startRedLights();
        playSound('alarm', { volume: 0.7},() => {
            playSound('alarm', { volume: 1.0},() => {
                playSound('alarm', { volume: 0.7},() => {
                    currentState = STATES.CLOSED;
                    ignore = false;
                    stopRedLights();
                });
            })
        })
    }

    let openDoors = () => {
        playSound('door', { volume: 0.8 });
        nextState = STATES.OPEN;
        document.querySelector('#leftdoor').emit('open');
        document.querySelector('#rightdoor').emit('open');
    }

    let closeDoors = () => {
        playSound('door', { volume: 0.8 });
        nextState = STATES.CLOSED;
        document.querySelector('#leftdoor').emit('close');
        document.querySelector('#rightdoor').emit('close');
    }

    window.addEventListener('keydown', e => {
        //
    });
    window.addEventListener('keyup', e => {
        if(!ignore) {
            if(e.keyCode === 32) {
                closeDoorEvent();
            }
            if(e.keyCode === 13) {
                openDoorEvent();
            }
            if(e.keyCode === 82) {
                badReadEvent();
            }
        }
    });

    window.addEventListener('animationend', e => {
        ignore = false;        
        if(nextState !== null) {
            currentState = nextState;
            nextState = null;
        }
    });
});
