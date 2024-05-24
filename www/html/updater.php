<?php
require('conf.php');

while (true) {
    sleep(1);
    $status = file_get_contents('/var/www/webview/status.conf');
    if ($status !== 'on')
        continue;
    $temp = file_get_contents('/var/www/webview/temperature.txt');
    if (empty($temp) && $temp !== '0')
        continue;
    $temp = intval($temp) + mt_rand() / mt_getrandmax();
    $stmt = $pdo->prepare('INSERT INTO temperatures (temperature) VALUES (?)');
    $stmt->execute([$temp]);
}