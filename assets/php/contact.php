<?php

$con = mysqli_connect("localhost", "root", "6J(9u[0(m16^", "comeng_server_database");
if (!$con) {
    die("connection failed: " . mysqli_connect_error());
}


$fname = $_POST['fname'];
$lname = $_POST['lname'];
$grade = $_POST['grade'];
$email = $_POST['email'];
$msg = $_POST['msg'];

$sql = "INSERT INTO `contact_requests` (`first_name`, `last_name`, `grade`, `email`, `message`) VALUES ('$fname', '$lname', '$grade', '$email', '$msg')";

$rs = mysqli_query($con, $sql);

if ($rs) {
    echo "contact requests recorded";
}

?>