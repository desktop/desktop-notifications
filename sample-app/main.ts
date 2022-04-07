import { app, BrowserWindow } from 'electron'
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

  initializeNotifications('{27D44D0C-A542-5B90-BCDB-AC3126048BA2}')

  const n1 = new DesktopNotification('First notification', 'This is a test')
  n1.onclick = () => {
    console.log('1 was clicked!!!')
  }
  n1.show()

  console.log('permission?', getNotificationsPermission())
})
