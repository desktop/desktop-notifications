import { ipcRenderer } from 'electron'
import QuickLRU from 'quick-lru'

const notificationCallbacks = new QuickLRU<string, () => void>({
  maxSize: 200,
})

ipcRenderer.on('notification-event', (event, id, userInfo) => {
  console.log(`[RENDERER] Notification event: ${event} ${id}`, userInfo)

  const callback = notificationCallbacks.get(id)
  callback?.()
  notificationCallbacks.delete(id)
})

setupClickEventListener('showNotificationButton', showNotification)
setupClickEventListener('checkPermissionsButton', checkNotificationsPermission)
setupClickEventListener(
  'requestPermissionsButton',
  requestNotificationsPermission
)

function setupClickEventListener(id: string, onclick: () => void) {
  const element = document.getElementById(id)
  if (element === null) {
    throw new Error(`Could not find notification with id ${id}`)
  }
  element.addEventListener('click', onclick)
}

async function showNotification() {
  const notificationID = await ipcRenderer.invoke(
    'show-notification',
    'Test notification',
    'This notification is a test. Hello! 👋',
    {
      myDict: { foo: 'bar' },
      myString: 'Hello world!',
      myBool: true,
      myArray: ['one', 2, 'three'],
      type: 'pr-review-submit-fake',
      myNumber: 42,
    }
  )

  console.log('Shown notification with ID:', notificationID)

  notificationCallbacks.set(notificationID, () => {
    console.log(`[RENDERER] Notification clicked: ${notificationID}`)
  })
}

async function checkNotificationsPermission() {
  console.log(
    'checkPermissions',
    await ipcRenderer.invoke('get-notifications-permission')
  )
}

async function requestNotificationsPermission() {
  console.log(
    'requestPermissions',
    await ipcRenderer.invoke('request-notifications-permission')
  )
}
