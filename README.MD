<p align="center">
  <a href="https://github.com/swiftly-solution/swiftly_shop">
    <img src="https://cdn.swiftlycs2.net/swiftly-logo.png" alt="SwiftlyLogo" width="80" height="80">
  </a>

  <h3 align="center">[Swiftly] Shop System</h3>

  <p align="center">
    A simple plugin for Swiftly that implements an Shop System (WIP).
    <br/>
  </p>
</p>

<p align="center">
  <img src="https://img.shields.io/github/downloads/swiftly-solution/swiftly_shop/total" alt="Downloads"> 
  <img src="https://img.shields.io/github/contributors/swiftly-solution/swiftly_shop?color=dark-green" alt="Contributors">
  <img src="https://img.shields.io/github/issues/swiftly-solution/swiftly_shop" alt="Issues">
  <img src="https://img.shields.io/github/license/swiftly-solution/swiftly_shop" alt="License">
</p>

---

### Installation 👀

1. Download the newest [release](https://github.com/swiftly-solution/swiftly_shop/releases).
2. Everything is drag & drop, so i think you can do it!
3. Setup database connection in `addons/swiftly/configs/databases.json` with the key `swiftly_shop` like in the following example:
```json
{
    "swiftly_shop": {
        "host": "...",
        "username": "...",
        "password": "...",
        "database": "...",
        "port": 3306
    }
}
```
(!) Don't forget to replace the `...` with the actual values !!

### Configuring the plugin 🧐

* After installing the plugin, you should change the default values from ``addons/swiftly/configs/swiftly_shop.json``

### IMPORTANT ❗

* This is a WIP plugin, all of the "modules" will be implemented right onto the plugin, it will not be a modular plugin.

### FEATURES 📙

* Simple HP / Kill system, everything configurable through ``addons/swiftly_configs/swiftly_shop.json``.

### TODO 🛡️

* Add features to the shop (Medic, TAGS, more events etc.).


### Creating A Pull Request 😃

1. Fork the Project
2. Create your Feature Branch
3. Commit your Changes
4. Push to the Branch
5. Open a Pull Request

### Have ideas/Found bugs? 💡
Join [Swiftly Discord Server](https://swiftlycs2.net/discord) and send a message in the topic from `📕╎plugins-sharing` of this plugin!

---