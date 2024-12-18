const serverAddress = window.location.hostname;
const serverPort = window.location.port;

// Construct the WebSocket URL (use wss for HTTPS)
const ws = new WebSocket(`wss://${serverAddress}:${serverPort}`);
const remoteVideo = document.getElementById('remoteVideo');
let peerConnection;

const config = {
    iceServers: [
        { urls: 'stun:stun.l.google.com:19302' },
        { urls: 'stun:stun1.l.google.com:19302' },
        { urls: 'stun:stun2.l.google.com:19302' },
        { urls: 'stun:stun3.l.google.com:19302' },
        { urls: 'stun:stun4.l.google.com:19302' },
    ]
};

function createPeerConnection() {
    console.log("Creating RTCPeerConnection");
    try {
        peerConnection = new RTCPeerConnection(config);

        peerConnection.ontrack = event => {
            console.log("Got track", event);
            if (event.streams && event.streams[0]) {
                console.log("Setting remote stream");
                remoteVideo.srcObject = event.streams[0];
            } else {
                console.log("Adding track to new MediaStream");
                const inboundStream = new MediaStream([event.track]);
                remoteVideo.srcObject = inboundStream;
            }
        };

        peerConnection.onicecandidate = event => {
            console.log("ICE candidate:", event.candidate);
            if (event.candidate) {
                ws.send(JSON.stringify({ type: 'ice_candidate', candidate: event.candidate }));
            }
        };

        peerConnection.onconnectionstatechange = event => {
            console.log("Connection state change:", peerConnection.connectionState);
        };

        peerConnection.onicegatheringstatechange = event => {
            console.log("ICE gathering state change:", peerConnection.iceGatheringState);
        };

        console.log("RTCPeerConnection created successfully");
    } catch (error) {
        console.error("Error creating RTCPeerConnection:", error);
    }
}

ws.onopen = () => {
    console.log('WebSocket connected');
    // Register as a server-view client
    ws.send(JSON.stringify({ type: 'register', role: 'server-view' }));
    createPeerConnection();
};

ws.onmessage = (event) => {
    const data = JSON.parse(event.data);
    console.log("Received message:", data);

    if (data.type === 'offer') {
        console.log("Got offer");
        peerConnection.setRemoteDescription(new RTCSessionDescription(data.offer))
            .then(() => {
                console.log("Creating answer");
                return peerConnection.createAnswer();
            })
            .then(answer => {
                console.log("Setting local description");
                return peerConnection.setLocalDescription(answer);
            })
            .then(() => {
                console.log("Sending answer");
                ws.send(JSON.stringify({ type: 'answer', answer: peerConnection.localDescription }));
            })
            .catch(error => {
                console.error("Error handling offer:", error);
            });
    } else if (data.type === 'ice_candidate') {
        if (peerConnection) {
            console.log("Adding ICE candidate");
            peerConnection.addIceCandidate(new RTCIceCandidate(data.candidate))
                .catch(error => {
                    console.error("Error adding ICE candidate:", error);
                });
        }
    }
};

ws.onerror = (error) => {
    console.error('WebSocket error:', error);
};