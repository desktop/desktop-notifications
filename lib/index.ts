export {
  initializeNotifications,
  showNotification,
  closeNotification,
  terminateNotifications,
  getNotificationsPermission,
  requestNotificationsPermission,
} from './native-module'
export {
  supportsNotifications,
  supportsNotificationsPermissionRequest,
} from './notification-support'
export { onNotificationEvent } from './notification-callback'
