const serverAddress = window.location.hostname;
const serverPort = window.location.port;

// Construct the WebSocket URL (use wss for HTTPS)
const ws = new WebSocket(`wss://${serverAddress}:${serverPort}`);
const videoElement = document.getElementById('localVideo');
const canvasElement = document.getElementById('hiddenCanvas');
const canvasCtx = canvasElement.getContext('2d');
const remoteAudio = document.getElementById('remoteAudio'); // Get the audio element
let localStream;
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

async function startCamera() {
    try {
        const devices = await navigator.mediaDevices.enumerateDevices();
        console.log("Available devices:", devices);

        const videoDevices = devices.filter(device => device.kind === 'videoinput');

        if (videoDevices.length > 0) {
            const selectedDeviceId = videoDevices[0].deviceId;

            localStream = await navigator.mediaDevices.getUserMedia({
                video: { deviceId: selectedDeviceId },
                audio: true
            });

            videoElement.srcObject = localStream;
            videoElement.muted = true; // Mute the local video element
            canvasElement.width = videoElement.videoWidth;
            canvasElement.height = videoElement.videoHeight;
            drawFlippedVideoToCanvas();

        } else {
            console.error('No video input devices found.');
        }
    } catch (error) {
        console.error('Error accessing media devices.', error);
    }
}

function createPeerConnection() {
    console.log("Creating RTCPeerConnection with config:", config);
    try {
        peerConnection = new RTCPeerConnection(config);

        // Add local stream's tracks to peer connection
        localStream.getTracks().forEach(track => {
            console.log("Adding track to peer connection:", track);
            peerConnection.addTrack(track, localStream);
        });

        peerConnection.ontrack = event => {
            console.log("Got track", event);
            if (event.streams && event.streams[0]) {
                console.log("Setting remote stream");
                if (event.track.kind === 'audio') {
                    console.log("Setting remote audio stream");
                    remoteAudio.srcObject = event.streams[0];
                }
            } else {
                console.log("Adding track to new MediaStream");
                const inboundStream = new MediaStream([event.track]);
                if (event.track.kind === 'audio') {
                    console.log("Setting remote audio stream");
                    remoteAudio.srcObject = inboundStream;
                }
            }
        };

        peerConnection.onicecandidate = event => {
            console.log("ICE candidate:", event.candidate);
            if (event.candidate) {
                ws.send(JSON.stringify({ type: 'ice_candidate', candidate: event.candidate }));
            }
        };

        peerConnection.onnegotiationneeded = async () => {
            console.log("Negotiation needed");
            try {
                console.log("Creating offer");
                const offer = await peerConnection.createOffer();
                console.log("Setting local description");
                await peerConnection.setLocalDescription(offer);
                console.log("Sending offer");
                ws.send(JSON.stringify({ type: 'offer', offer: peerConnection.localDescription }));
            } catch (error) {
                console.error("Error creating offer:", error);
            }
        };

        console.log("RTCPeerConnection created successfully");
    } catch (error) {
        console.error("Failed to create RTCPeerConnection:", error);
        return;
    }
}

function drawFlippedVideoToCanvas() {
    canvasCtx.translate(canvasElement.width, 0);
    canvasCtx.scale(-1, 1);
    canvasCtx.drawImage(videoElement, 0, 0, canvasElement.width, canvasElement.height);
    requestAnimationFrame(drawFlippedVideoToCanvas);
}

ws.onopen = () => {
    console.log('WebSocket connected');
    ws.send(JSON.stringify({ type: 'register', role: 'open-cam' }));
    startCamera();
    // Create peer connection after starting camera and registering
    setTimeout(createPeerConnection, 1000);
};

ws.onmessage = (event) => {
    const data = JSON.parse(event.data);
    console.log("Received message:", data);

    if (data.type === 'answer') {
        console.log("Got answer");
        peerConnection.setRemoteDescription(new RTCSessionDescription(data.answer))
            .catch(error => console.error("Error setting remote description:", error));
    } else if (data.type === 'ice_candidate') {
        if (peerConnection) {
            console.log("Adding ICE candidate");
            peerConnection.addIceCandidate(new RTCIceCandidate(data.candidate))
                .catch(error => console.error("Error adding ICE candidate:", error));
        }
    }
};

ws.onerror = (error) => {
    console.error('WebSocket error:', error);
};