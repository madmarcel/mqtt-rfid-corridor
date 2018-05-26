'use strict';

window.addEventListener('load', () => {
    
    let ignore = false;
    
    const STATES = {
        'CLOSED': 0,
        'OPENING': 1,
        'OPEN': 2,
        'CLOSING': 3
    };

    let currentState = STATES.CLOSED;
    let nextState = null;

    let compareState = (state) => {
        return currentState === STATES[state];
    }

    let isOpen = () => {
        return compareState('OPEN');
    }

    let isClosed = () => {
        return compareState('CLOSED');
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
                if(isOpen()) {
                    playSound('bloop', { volume: 1.0 });
                    ignore = true;
                    currentState = STATES.CLOSING;
                    setTimeout(closeDoors, 400);
                } else {
                    playSound('click', { volume: 1.0 });
                }
            }
            if(e.keyCode === 13) {
                if(isClosed()) {
                    playSound('successread', { volume: 1.0 });
                    ignore = true;
                    currentState = STATES.OPENING;
                    setTimeout(openDoors, 400);
                } else {
                    playSound('ignoreread', { volume: 1.0 });
                }
            }
            if(e.keyCode === 64) {
                
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
