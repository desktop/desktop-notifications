import { app, BrowserWindow, ipcMain } from 'electron'
import path from 'path'
import * as url from 'url'
import {
  getNotificationsPermission,
  initializeNotifications,
  showNotification,
  onNotificationEvent,
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
  return win
}

app.whenReady().then(() => {
  const window = createWindow()

  initializeNotifications({
    toastActivatorClsid: '{27D44D0C-A542-5B90-BCDB-AC3126048BA2}',
  })

  onNotificationEvent((event, id, userInfo) => {
    console.log(`[MAIN] Notification event: ${event} ${id}`, userInfo)
    window.webContents.send('notification-event', event, id, userInfo)
  })

  ipcMain.handle(
    'show-notification',
    (_, title: string, body: string, userInfo: Record<string, any>) => {
      return showNotification(title, body, userInfo)
    }
  )

  ipcMain.handle('get-notifications-permission', async () => {
    return getNotificationsPermission()
  })
})
