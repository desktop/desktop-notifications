import {
  initializeNotifications,
  terminateNotifications,
  DesktopNotification,
} from '../lib/desktop-notifications'
;(async () => {
  initializeNotifications('{27D44D0C-A542-5B90-BCDB-AC3126048BA2}')

  const n1 = new DesktopNotification('First notification', 'This is a test')
  n1.onclick = () => {
    console.log('1 was clicked!!!')
  }
  n1.show()

  const n2 = new DesktopNotification(
    'Second notification',
    'This is another test'
  )
  n2.onclick = () => {
    console.log('2 was clicked!!!')
  }
  n2.show()

  const n3 = new DesktopNotification(
    'Third notification',
    'This is the 3rd test'
  )
  n3.onclick = () => {
    console.log('3 was clicked!!!')
  }
  n3.show()

  console.log(`start waiting`)
  await new Promise(r => setTimeout(r, 5000))
  n2.close()
  await new Promise(r => setTimeout(r, 5000))
  n2.close()
  n2.close()
  n2.close()
  console.log('finished waiting')

  terminateNotifications()
})()
