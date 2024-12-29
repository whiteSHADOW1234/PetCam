const express = require('express');
const https = require('https');
const WebSocket = require('ws');
const path = require('path');
const ip = require('ip');
const os = require('os');
const fs = require('fs');
const axios = require('axios');

const app = express();

// --- HTTPS Setup ---
const options = {
    key: fs.readFileSync(path.join(__dirname, 'key.pem')),
    cert: fs.readFileSync(path.join(__dirname, 'cert.pem'))
};
const server = https.createServer(options, app);
const wss = new WebSocket.Server({ server });

// --- Client Tracking ---
let openCamClient = null;
let serverViewClients = []; // Array to store connected server-view clients

// --- WebSocket Connection Handling ---
wss.on('connection', (ws, req) => {
    const clientIp = req.socket.remoteAddress;
    console.log(`Client connected from IP: ${clientIp}`);

    ws.on('message', (message) => {
        const data = JSON.parse(message);
        console.log(`Received message from ${clientIp}:`, data);

        if (data.type === 'register') {
            if (data.role === 'open-cam') {
                openCamClient = ws;
                console.log(`Registered open-cam client from IP: ${clientIp}`);
            } else if (data.role === 'server-view') {
                serverViewClients.push(ws);
                console.log(`Registered server-view client from IP: ${clientIp}`);
            }
        } else if (data.type === 'offer' && openCamClient === ws) {
            // Relay offer to the server-view clients
            console.log('Relaying offer to server-view clients');
            serverViewClients.forEach(client => {
                if (client.readyState === WebSocket.OPEN) {
                    client.send(JSON.stringify(data));
                }
            });
        } else if (data.type === 'answer' && serverViewClients.includes(ws)) {
            // Relay answer to open-cam client
            console.log('Relaying answer to open-cam client');
            if (openCamClient && openCamClient.readyState === WebSocket.OPEN) {
                openCamClient.send(JSON.stringify(data));
            }
        } else if (data.type === 'ice_candidate') {
            // Relay ICE candidates to the appropriate client (either open-cam or server-view)
            if (openCamClient === ws) {
                console.log('Relaying ICE candidate to server-view clients');
                serverViewClients.forEach(client => {
                    if (client.readyState === WebSocket.OPEN) {
                        client.send(JSON.stringify(data));
                    }
                });
            } else if (serverViewClients.includes(ws)) {
                console.log('Relaying ICE candidate to open-cam client');
                if (openCamClient && openCamClient.readyState === WebSocket.OPEN) {
                    openCamClient.send(JSON.stringify(data));
                }
            }
        }
    });

    ws.on('close', () => {
        console.log(`Client disconnected: ${clientIp}`);
        if (ws === openCamClient) {
            openCamClient = null;
        }
        serverViewClients = serverViewClients.filter(client => client !== ws);
    });
});

// --- Serve Static Files ---
app.use('/client', express.static(path.join(__dirname, '../client')));

// --- Route Handlers ---
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'templates', 'index.html')); // Serve the landing page
});

app.get('/client/open-cam', (req, res) => {
    res.sendFile(path.join(__dirname, '../client/open-cam/index.html'));
});

app.get('/server-view', (req, res) => {
    res.sendFile(path.join(__dirname, 'templates', 'server-view.html')); // Serve the server view page
});

// --- Function to Get All Accessible IP Addresses ---
function getAccessibleIpAddresses() {
    const interfaces = os.networkInterfaces();
    const ipAddresses = [];

    for (const interfaceName in interfaces) {
        const iface = interfaces[interfaceName];
        for (const alias of iface) {
            if (alias.family === 'IPv4' && alias.address !== '127.0.0.1' && !alias.internal) {
                ipAddresses.push(alias.address);
            }
        }
    }

    return ipAddresses;
}

// --- Control Motor via ESP32 ---
app.post('/control-motor', (req, res) => {
    const esp32Ip = req.query.ip;
    const direction = req.query.direction; // 取得 direction 參數

    let message = '';
    if (direction === 'clockwise') {
        message = 'clockwise';
    } else if (direction === 'counterclockwise') {
        message = 'counterclockwise';
    } else {
        res.status(400).send('Invalid direction');
        return;
    }

    axios.post(`http://${esp32Ip}/message`, message)
        .then(response => {
            console.log('Motor control command sent to ESP32', response.data);
            res.send('Motor control command sent to ESP32');
        })
        .catch(error => {
            console.error('Error sending command to ESP32', error);
            res.status(500).send('Error sending command to ESP32');
        });
});

// 新增的 /test 路由，用於測試與 ESP32 的連線
app.get('/test', (req, res) => {
    const esp32Ip = req.query.ip;
    if (!esp32Ip) {
        return res.status(400).send('ESP32 IP is required');
    }

    axios.get(`http://${esp32Ip}/test`)
        .then(response => {
            console.log('ESP32 test successful:', response.data);
            res.send(response.data); // 將 ESP32 的回應傳回給客戶端
        })
        .catch(error => {
            console.error('ESP32 test failed:', error);
            res.status(500).send('ESP32 test failed');
        });
});

// --- Start the Server ---
const PORT = process.env.PORT || 5000;
const HOST = '0.0.0.0';

server.listen(PORT, HOST, () => {
    const ipAddresses = getAccessibleIpAddresses();
    console.log(`Server started on https://${HOST}:${PORT}`);

    ipAddresses.forEach(ipAddress => {
        console.log(`Accessible URLs for IP address ${ipAddress}:`);
        console.log(`- Open Cam: https://${ipAddress}:${PORT}/client/open-cam`);
        console.log(`- Server View: https://${ipAddress}:${PORT}/server-view`);
    });
});