import {
  DesktopNotification,
  getNotificationsPermission,
  initializeNotifications,
} from 'desktop-notifications'

setupClickEventListener('showNotificationButton', showNotification)
setupClickEventListener('requestPermissionsButton', requestPermissions)
setupClickEventListener('checkPermissionsButton', checkPermissions)

function setupClickEventListener(id: string, onclick: () => void) {
  const notification = document.getElementById(id)
  if (notification === null) {
    throw new Error(`Could not find notification with id ${id}`)
  }
  notification.addEventListener('click', onclick)
}

initializeNotifications('{27D44D0C-A542-5B90-BCDB-AC3126048BA2}')

function showNotification() {
  console.log('showNotification')

  const n1 = new DesktopNotification('First notification', 'This is a test')
  n1.onclick = () => {
    console.log('1 was clicked!!!')
  }
  n1.show()
}

function requestPermissions() {
  console.log('requestPermissions', getNotificationsPermission())
}

function checkPermissions() {
  console.log('checkPermissions')
}
