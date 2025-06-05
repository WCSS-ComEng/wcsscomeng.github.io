<?php
// Database connection
$servername = "localhost";
$username = "root";
$password = "dolphindatabaseworkbench";
$dbname = "comeng_server_database";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Check if the form is submitted via POST
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Retrieve slider values from POST request
    $slider1 = isset($_POST['slider1']) ? (int)$_POST['slider1'] : 0;
    $slider2 = isset($_POST['slider2']) ? (int)$_POST['slider2'] : 0;
    $slider3 = isset($_POST['slider3']) ? (int)$_POST['slider3'] : 0;

    // Prepare and bind
    $stmt = $conn->prepare("INSERT INTO slider_values (slider1, slider2, slider3, last_modified) VALUES (?, ?, ?, NOW())");
    $stmt->bind_param("iii", $slider1, $slider2, $slider3);

    // Execute the statement
    if ($stmt->execute()) {
        echo "New records created successfully";
    } else {
        echo "Error: " . $stmt->error;
    }

    // Close connections
    $stmt->close();
} else {
    // Handle the case where the form is not submitted via POST
    echo "Please submit the form.";
}

$conn->close();
?>
