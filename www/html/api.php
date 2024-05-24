<?php
require('conf.php');

if (isset($_GET['topToday'])){
    $stmt = $pdo->prepare('SELECT temperature, created_at FROM temperatures WHERE DATE(created_at) = CURDATE() ORDER BY temperature DESC LIMIT 10');
    $stmt->execute();
    echo(json_encode($stmt->fetchAll()));
}
else if (isset($_GET['topHour'])){
    $stmt = $pdo->prepare('SELECT temperature, created_at FROM temperatures WHERE created_at >= NOW() - INTERVAL 1 HOUR ORDER BY temperature DESC LIMIT 10');
    $stmt->execute();
    echo(json_encode($stmt->fetchAll()));
}
else if (isset($_GET['topMinute'])){
    $stmt = $pdo->prepare('SELECT temperature, created_at FROM temperatures WHERE created_at >= NOW() - INTERVAL 1 MINUTE ORDER BY temperature DESC LIMIT 10');
    $stmt->execute();
    echo(json_encode($stmt->fetchAll()));
}
else if (isset($_GET['last'])){
    $stmt = $pdo->prepare('SELECT temperature, created_at FROM temperatures ORDER BY id DESC LIMIT 30');
    $stmt->execute();
    echo(json_encode($stmt->fetchAll()));
}
else if (isset($_GET['clear'])){
    $stmt = $pdo->prepare('DELETE FROM temperatures');
    $stmt->execute();
}
else if (isset($_GET['stop'])){
    file_put_contents('status.conf', 'off');
}
else if (isset($_GET['start'])){
    file_put_contents('status.conf', 'on');
}