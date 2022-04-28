import { app, BrowserWindow, ipcMain } from 'electron'
import path from 'path'
import * as url from 'url'
import {
  DesktopNotification,
  getNotificationsPermission,
  initializeNotifications,
} from 'desktop-notifications'

const createWindow = () => {
  const win = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      nodeIntegration: true,
      contextIsolation: false,
    },
  })

  win.loadURL(
    url.format({
      pathname: path.join(__dirname, '../index.html'),
      protocol: 'file:',
      slashes: true,
    })
  )
  win.webContents.openDevTools()
}

app.whenReady().then(() => {
  createWindow()

  initializeNotifications({
    toastActivatorClsid: '{27D44D0C-A542-5B90-BCDB-AC3126048BA2}',
  })

  // const n2 = new DesktopNotification('Second notification', 'This is a test')
  // n2.onclick = () => {
  //   console.log('2 was clicked!!!')
  // }
  // n2.show()

  ipcMain.on('show-notification', () => {
    const n2 = new DesktopNotification(
      'Notification title here',
      'This is a test'
    )
    n2.onclick = () => {
      console.log('notification was clicked!!!')
    }
    n2.show()
  })

  ipcMain.handle('get-notifications-permission', async () => {
    return getNotificationsPermission()
  })
})
