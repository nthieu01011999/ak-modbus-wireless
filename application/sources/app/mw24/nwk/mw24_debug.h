#ifndef __MW24_DEBUG_H__
#define __MW24_DEBUG_H__

/******************************************************************************
 * DEBUG
*******************************************************************************/
/* SET DEBUG - IO */
#define MW24_DEBUG_TX_EN		0
#define MW24_DEBUG_RX_EN		0
#define MW24_DEBUG_IRQ_EN		0

/* SET DEBUG - CONSOLE */
#define MW24_DEBUG_FLAGS_EN		0
#define MW24_DEBUG_DATA_EN		0
#define MW24_DEBUG_STATUS_EN	0

/******************************************************************************
 * DEBUG - IO
*******************************************************************************/
#if (MW24_DEBUG_TX_EN == 1)
#define MW24_DEBUG_TX_ON() do { mw24_tx_on(); } while (0)
#else
#define MW24_DEBUG_TX_ON()
#endif

#if (MW24_DEBUG_TX_EN == 1)
#define MW24_DEBUG_TX_OFF() do { mw24_tx_off(); } while (0)
#else
#define MW24_DEBUG_TX_OFF()
#endif

#if (MW24_DEBUG_RX_EN == 1)
#define MW24_DEBUG_RX_ON() do { mw24_rx_on(); } while (0)
#else
#define MW24_DEBUG_RX_ON()
#endif

#if (MW24_DEBUG_RX_EN == 1)
#define MW24_DEBUG_RX_OFF() do { mw24_rx_off(); } while (0)
#else
#define MW24_DEBUG_RX_OFF()
#endif

#if (MW24_DEBUG_IRQ_EN == 1)
#define MW24_DEBUG_IRQ_ON() /*do {  } while (0)*/
#else
#define MW24_DEBUG_IRQ_ON()
#endif

#if (MW24_DEBUG_IRQ_EN == 1)
#define MW24_DEBUG_IRQ_OFF() /*do {  } while (0)*/
#else
#define MW24_DEBUG_IRQ_OFF()
#endif

/******************************************************************************
 * DEBUG - CONSOLE
*******************************************************************************/
#if (MW24_DEBUG_FLAGS_EN == 1)
#define MW24_DEBUG_FLAGS(fmt, ...)            xprintf("RF-FLAGS-> " fmt, ##__VA_ARGS__)
#else
#define MW24_DEBUG_FLAGS(fmt, ...)
#endif

#if (MW24_DEBUG_DATA_EN == 1)
#define MW24_DEBUG_DATA(fmt, ...)             xprintf("RF-DATA-> " fmt, ##__VA_ARGS__)
#else
#define MW24_DEBUG_DATA(fmt, ...)
#endif

#if (MW24_DEBUG_STATUS_EN == 1)
#define MW24_DEBUG_STATUS(fmt, ...)           xprintf("RF-STATUS-> " fmt, ##__VA_ARGS__)
#else
#define MW24_DEBUG_STATUS(fmt, ...)
#endif

#endif //__MW24_DEBUG_H__
