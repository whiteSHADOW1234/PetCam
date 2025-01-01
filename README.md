# PetCam: A Real-time Webcam Streaming with WebRTC
This project demonstrates a real-time webcam streaming application using WebRTC for video and audio transmission. It consists of a server, an "open-cam" client (the source of the webcam stream), and a "server-view" client (a web page to display the stream).

## Features

*   **WebRTC-based Streaming:** Utilizes WebRTC for efficient, low-latency video and audio streaming.
*   **Single Camera Source:** A single designated client (`open-cam`) captures and streams the webcam.
*   **Server-Side Relay:** The server acts as a signaling server to facilitate WebRTC connection establishment between `open-cam` and `server-view` clients.
*   **Web-based Viewer:** A simple web page (`server-view`) displays the received video stream.
*   **HTTPS:** Uses HTTPS for secure communication and to comply with browser security requirements for `getUserMedia`.
*   **No External Media Server:** For simplicity and testing purposes, this project does not rely on an external media server like Kurento or Janus.

## Project Structure
```
PetCam/
├── server/
│ ├── server.js # Node.js server with WebRTC signaling
│ ├── key.pem # SSL private key (self-signed for testing)
│ └── cert.pem # SSL certificate (self-signed for testing)
│ └── templates/
│   ├── index.html # Server landing page
│   └── server-view.html # Website to view stream
├── client/
│ └── open-cam/
│   ├── index.html # Website to start camera
│   └── script.js # JavaScript to handle camera, WebRTC, and streaming
│     └── server-view/
│       └── script.js # JavaScript for WebRTC receiving logic
└── esp_idf_esp32s3/
  ├── .devcontainer/
  ├── test/
  └── main/
    ├── CMakeLists.txt
    ├── http.cpp
    ├── http.h
    ├── joystick.cpp
    ├── joystick.h
    ├── main.cpp
    ├── wifi.cpp
    ├── wifi.h
    ├── ULN2003.cpp
    └── ULN2003.h
```

## Prerequisites

*   **Node.js and npm:** Make sure you have Node.js and npm (or yarn) installed on your server machine.
*   **OpenSSL:** You'll need OpenSSL to generate self-signed certificates for HTTPS. It's often included with Git on Windows and comes pre-installed on most Linux/macOS systems.
*   **Web Browser:** A modern web browser that supports WebRTC (e.g., Brave, Firefox, Edge). **You might see an "Advanced" button; click it, then click "Accept the Risk and Continue." Also don't forget to grant camera and microphone permissions.**

> [!NOTE]  
> **DO NOT USE Chrome AS YOUR WEB BROWSER, OR THIS ENTIRE PROJECT WILL FAIL !!!!!**

## Installation

1. **Clone the repository:**

    ```bash
    git clone https://github.com/whiteSHADOW1234/PetCam.git
    cd PetCam
    ```

2. **Install dependencies:**

    ```bash
    npm install
    ```

3. **Generate Self-Signed Certificate:**

    *   If you don't have OpenSSL, install it (e.g., `sudo apt install openssl` on Debian/Ubuntu).
    *   Run the following command in the root directory, **replacing `192.168.1.10` with your server's actual LAN IP address**:

        ```bash
        openssl req -x509 -newkey rsa:4096 -keyout server/key.pem -out server/cert.pem -sha256 -days 365 -nodes -subj "/C=US/ST=CA/L=San Francisco/O=MyOrg/CN=192.168.1.10" -addext "subjectAltName=IP:192.168.1.10,DNS:localhost,IP:127.0.0.1" -addext "extendedKeyUsage=serverAuth"
        ```

    *   This will create `key.pem` (private key) and `cert.pem` (certificate) files in the `server` directory.

## Running the Application

1. **Start the Server:**

    ```bash
    npm start
    ```

    The console will output all URLs you can use to access the `open-cam` and `server-view` pages using your server's LAN IP address.

2. **Access the Open-Cam Website:**

    *   Open a web browser on the device with the webcam you want to use.
    *   Go to `https://<avaliable_LAN_ip>:5000/client/open-cam`.
    *   Replace `<avaliable_LAN_ip>` with your server's actual LAN IP address.
    *   Grant the website permission to access your camera and microphone when prompted by the browser.

3. **Access the Server View Website:**

    *   Open another web browser (on any device on your LAN).
    *   Go to `https://<avaliable_LAN_ip>:5000/server-view`.
    *   You should see the video stream from the `open-cam` device.

> [!IMPORTANT]  
>
> *   **HTTPS:** Access both websites using `https://` to ensure secure connections and allow `getUserMedia` to work correctly.
> *   **Self-Signed Certificate:** Since you're using a self-signed certificate, your browser will display a security warning. You'll need to add a temporary exception in your browser to proceed during development. **For production, you must use a valid certificate from a trusted Certificate Authority (CA).**
> *   **Firewall:** Make sure your server's firewall allows incoming connections on port 5000.
> *   **LAN Access:** The provided URLs will only work for devices on the same local network (LAN) as your server.
> *   **Performance:** The performance of the video stream might vary depending on network conditions and the encoding/decoding process.
> *   **Audio:** This current implementation does not include audio streaming, which will require further development.
> *   **Scalability:** This setup is suitable for testing and a limited number of viewers. For a more scalable solution, consider using a dedicated media server like Kurento, Janus, or Jitsi.

## Troubleshooting

*   **Camera Not Starting:**
    *   Verify browser and operating system permissions for camera access.
    *   Check for errors in the browser's developer console (F12).
    *   Make sure no other applications are using the camera.
    *   Try a different browser or device.
*   **Server Not Accessible:**
    *   Double-check your server's LAN IP address.
    *   Ensure the server is running and listening on port 5000.
    *   Check your firewall settings.
*   **No Video on Server View:**
    *   Examine the console logs of the `open-cam` browser, the `server-view` browser, and the server itself for any error messages.
    *   Use the browser's "Network" tab (in developer tools) to inspect WebSocket messages and check if the offer, answer, and ICE candidates are being exchanged correctly.
    *   Verify that the `RTCPeerConnection` on the `server-view` side is successfully established and that the `ontrack` event is firing.
    *   If there's no error found, refresh both `open-cam` and `server-view` site until this issue was solved.

## Future Improvements

*   **Multiple Open-Cam Clients:** Modify the server to handle multiple `open-cam` clients and allow selection of the desired stream on the `server-view` side.
*   **Security:** Implement proper authentication and authorization for a production environment.
*   **Scalability:** Integrate a dedicated media server (e.g., Kurento, Janus) for improved performance and scalability with multiple clients.
*   **Production Deployment:** Obtain a valid SSL certificate from a trusted CA, configure a domain name, and set up a robust deployment environment for production use.