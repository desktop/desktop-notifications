import * as win32 from './win32'
import * as darwin from './darwin'
export { DesktopNotification } from './shared/notification'

const platformModule = process.platform === 'win32' ? win32 : darwin

export const initializeNotifications = platformModule.initializeNotifications
export const getNotificationsPermission =
  platformModule.getNotificationsPermission
