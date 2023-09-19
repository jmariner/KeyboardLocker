const TouchPortalAPI = require("touchportal-api");
const pluginInfo = require("./plugin.json");

const TPClient = new TouchPortalAPI.Client();

TPClient.on("Action", (data) => {
    TPClient.logIt("INFO", "GOT ACTION", JSON.stringify(data));
    if (data.actionId === "keyboardlocker-link-set-locked") {
        /** @type {"Toggle"|"Enable"|"Disable"} */
        const { value } = data.data.find(x => x.id === "keyboardlocker-link-set-locked-state") || {};
        TPClient.logIt("INFO", "Keyboard Locker", `Set Locked: ${value}`);
    }
});

TPClient.on("Settings", (settings) => {
    TPClient.logIt("INFO", "GOT SETTINGS");
});

TPClient.connect({ pluginId: pluginInfo.id });