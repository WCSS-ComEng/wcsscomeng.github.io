<?php

$con = mysqli_connect("localhost","root","6J(9u[0(m16^","comeng_server_database");

$fname = $_POST['fname'];
$lname = $_POST['lname'];
$grade = $_POST['grade'];
$email = $_POST['email'];
$msg = $_POST['msg'];

$sql = "INSERT INTO `contact_requests` (`id`, `first_name`, `last_name`, `grade`, `email`, `message`) VALUES ('0', '$fname', '$lname', '$grade', '$email', '$msg')";

$rs = mysqli_query($con, $sql);

if($rs) {
	echo "contact requests recorded";
}

?>