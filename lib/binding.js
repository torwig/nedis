const addon = require('../build/Release/nedis');

function Nedis(name) {
    this.greet = function(str) {
        return _addonInstance.greet(str);
    }

    var _addonInstance = new addon.Nedis(name);
}

module.exports = Nedis;
