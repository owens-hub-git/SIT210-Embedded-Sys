import express from "express";
import IotApi from '@arduino/arduino-iot-client';
import { Client_Id_Code, Client_Secret_Code, Thing_Id_Code, LED1, LED2, LED3 } from "./things";

const app = express();

app.use(express.json());
app.use(express.static("public"));

const CLIENT_ID = Client_Id_Code;
const CLIENT_SECRET = Client_Secret_Code;
const THING_ID = Thing_Id_Code;


async function getToken() {
    const res = await fetch('https://api2.arduino.cc/iot/v1/clients/token', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded'
        },
        body: new URLSearchParams({
            grant_type: 'client_credentials',
            client_id: CLIENT_ID,
            client_secret: CLIENT_SECRET,
            audience: 'https://api2.arduino.cc/iot'
        })
    });

    if (!res.ok) {
        const err = await res.text();
        throw new Error("Token error: " + err);
    }

    const data = await res.json();
    return data.access_token;
}

const LEDS = {
    led1: LED1,
    led2: LED2,
    led3: LED3
};

async function publishProperty(thingId, propertyId, value) {
    const token = await getToken();

    const url = `https://api2.arduino.cc/iot/v2/things/${thingId}/properties/${propertyId}/publish`;

    const res = await fetch(url, {
        method: 'PUT',
        headers: {
            'Authorization': `Bearer ${token}`,
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            value: value
        })
    });

    if (!res.ok) {
        const err = await res.text();
        throw new Error("Publish failed: " + err);
    }

    console.log("Published successfully");
}

//API endpoint for button
app.post("/led", async (req, res) => {
    const { led, state } = req.body;

    try {
        const propertyId = LEDS[led];

        if (!propertyId) {
            return res.status(400).json({ error: "Invalid LED name" });
        }

        await publishProperty(THING_ID, propertyId, state);

        res.json({ ok: true, led, state });

    } catch (err) {
        res.status(500).json({ error: err.message });
    }
});


app.listen(3000, () => {
    console.log("Server running on http://localhost:3000");
});

//await listProperties();
//await publishProperty(THING_ID, propertyId, false);
