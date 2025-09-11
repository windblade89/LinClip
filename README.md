# **LinClip \- The Simple & Fast Clipboard Manager for Linux**

LinClip is a lightweight, efficient clipboard history manager built with C++ and Qt for Linux desktops. It runs quietly in the background, keeping a history of your copied text, and appears instantly at your mouse cursor with a global hotkey. It's designed to be a simple, no-fuss alternative to more complex clipboard tools.

## **✨ Features**

* **Global Hotkey:** Summon your clipboard history from any application by pressing Ctrl \+ Alt \+ V.  
* **Instant Access:** The window appears directly at your mouse cursor for quick interaction.  
* **Background Operation:** Runs as a system tray icon, staying out of your way.  
* **Simple Interface:** No complex features, just a clean list of your last 20 copied items.  
* **Efficient:** Built in C++ for minimal resource usage.

## **📦 Installation (for Users)**

These instructions are for Debian-based Linux distributions like **Ubuntu**, **Mint**, or **Pop\!\_OS**.

### **Step 1: Install Dependencies**

First, you need to install the Qt 6 development tools and a couple of required libraries. Open a terminal and run the following commands:

sudo apt update  
sudo apt install qt6-base-dev build-essential libx11-dev libxcb1-dev git

### **Step 2: Clone the Repository**

Next, clone this project's code from GitHub to your computer:

git clone https://github.com/windblade89/LinClip.git  
cd LinClip

### **Step 3: Compile the Application**

Now, we'll use the qmake tool to prepare the build files and make to compile the code into an executable.

qmake6  
make

### **Step 4: Run It\!**

An executable file named ClipboardManager has been created. You can run it directly from the terminal:

./build/Desktop-Debug/ClipboardManager

You should see the clipboard icon appear in your system tray. Now, try copying some text and pressing Ctrl \+ Alt \+ V\!

### **Step 5: Add to Applications Menu (Optional)**

To make LinClip launchable from your app dash, you can create a desktop entry.

1. **Move the executable** to a standard system directory. This makes it available everywhere. We'll also rename it to linclip.  
   sudo mv ./build/Desktop-Debug/ClipboardManager /usr/local/bin/linclip

2. **Create a desktop file.** This file tells your system about the new application.  
   nano \~/.local/share/applications/linclip.desktop

3. **Paste the following content** into the text editor:  
   \[Desktop Entry\]  
   Version=1.0  
   Type=Application  
   Name=LinClip  
   Comment=A simple and fast clipboard manager  
   Exec=/usr/local/bin/linclip  
   Icon=edit-copy  
   Terminal=false  
   Categories=Utility;

4. **Save and exit** by pressing Ctrl \+ X, then Y, and then Enter.

Your application should now appear when you search for "LinClip" in your applications menu. You might need to log out and log back in for it to show up.

## **🧑‍💻 Contributing (for Developers)**

Contributions are welcome\! Whether it's a bug fix, a new feature, or a documentation improvement, your help is appreciated.

### **Setup**

1. **Fork the repository** on GitHub.  
2. **Clone your fork** locally: git clone https://github.com/YOUR\_USERNAME/LinClip.git  
3. **Install Qt Creator:** The easiest way to work on the project is with the Qt IDE.  
   sudo apt install qtcreator

4. **Open the Project:** Launch Qt Creator and open the ClipboardManager.pro file.  
5. **Build and Run:** Qt Creator should automatically detect the configuration. Just click the green "Run" button to build and test the application.

### **How to Contribute**

1. Create a new branch for your feature or fix (git checkout \-b feature/my-new-feature).  
2. Make your changes and commit them with a clear message.  
3. Push your branch to your fork (git push origin feature/my-new-feature).  
4. Open a **Pull Request** from your fork to the main repository.

## **📜 License**

This project is licensed under the MIT License. See the LICENSE file for details.
