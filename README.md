![gluco-pc](https://github.com/user-attachments/assets/543aca45-7e8a-4507-a81c-04a6f4bc0d6f)
# Gluco-Monitor (DIY)

Gluco-Monitor is an open-source, low-cost device designed to display real-time glucose data from continuous glucose monitoring (CGM) systems such as **FreeStyle Libre** or **Dexcom**.

This project was originally developed to provide a simple, always-visible glucose display for everyday use — without needing to constantly check a smartphone.

---

## 🚀 Features

* 📊 Real-time glucose display
* 📈 Trend indication (rising, falling, stable)
* 🕒 History visualization at a glance
* 📡 Wireless data retrieval from CGM apps (e.g., Libre / Dexcom)
* 💡 Simple and clean interface
* 🔧 Fully DIY and open-source
* 💰 Low cost (~$25)

---

## 🧠 How It Works

Modern CGM systems like the FreeStyle Libre continuously measure glucose levels and transmit data to a smartphone every minute via Bluetooth ([FreeStyle][1]).

Gluco-Monitor connects to this ecosystem by retrieving glucose data (via compatible apps or APIs) and displaying it on a dedicated screen.

This allows users to:

* See their glucose instantly without unlocking their phone
* Monitor trends more easily
* Improve daily diabetes management

---

## 🛠️ Hardware

Typical components include:

* ESP32-based board
* Small TFT display
* Power supply (USB or battery)

👉 Total cost is typically around **$25**

---

## 💻 Software

The firmware is developed using:

* Arduino / PlatformIO
* Wi-Fi connectivity for data access
* Lightweight graphical interface

---

## 📦 Installation

1. Clone this repository:

```bash
git clone https://github.com/your-username/gluco-monitor.git
```

2. Open the project with PlatformIO or Arduino IDE

3. Upload to your ESP32 device
   
4. Configure your Wi-Fi and data source (Libre / Dexcom)

---

## 🔗 Documentation & Build Guide

Full step-by-step instructions are available here:

👉 https://f1atb.fr/gluco-monitor-diy/

This includes:

* Hardware assembly
* Wiring diagrams
* Firmware setup
* Configuration details

---

## ⚠️ Disclaimer

This project is **not a medical device** and must not be used for medical decisions.

Always rely on official CGM devices and medical advice for diabetes management.

---

## 🤝 Contributing

Contributions are welcome!

Feel free to:

* Open issues
* Suggest improvements
* Submit pull requests

---

## 📄 License

This project is open-source. See the LICENSE file for details.

---

## ❤️ Acknowledgments

* CGM community and open-source contributors
* Projects like xDrip+ and Nightscout
* Developers working on diabetes technology

---

## ⭐ Support the Project

If you like this project:

* ⭐ Star the repository
* Share it with others
* Build your own and contribute improvements!

---

[1]: https://pro.freestyle.abbott/ch-fr/gamme-freestyle/systemes-freestyle-libre/freestyle-libre-3.html?utm_source=chatgpt.com "FreeStyle Libre 3 System | Suisse | Professionnels de santé | Abbott"
