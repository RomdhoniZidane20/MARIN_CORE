
# NOTE MASTER

## 15 September 2022

    1. Untuk program slave saat ini masih belum bisa bulkread maupun syncwrite !!!
    2. Testing dengan protocol1.0 .
    3. Bulkread maupun snycwrite ditanggulani oleh latency timer usb menjadi 1 ms, maksimal latency yang bisa dicapai hingga 0 ms. (command : $ echo 1 | sudo tee /sys/bus/usb-serial/devices/ttyUSB0/latency_timer).
    4. Servo tidak bisa menjadi satu bus dengan slave esp karena akan terjadi masalah jika daya servo fail atau mati, paket akan error dan mengganggu paket yang dikirim oleh slave ke master (PC).
    
## 17 Januari 2023
    1. Slave BulkRead pada dynamixel2arduino slave upgrade memiliki maksimal panjang len address 6 byte. (PADA PROTOCOL 1.0)
    2. Jika bulkread address khusus read harus urut
    3. BELUM SUPPORT SYNCWRITE

## 25 Januari 2023
    1. Bulkread hanya bisa dibaca pada satu variabel array yang sama.
    2. Test untuk maksimal len address hingga 8 bit untuk paket Instruksi Bulkread.