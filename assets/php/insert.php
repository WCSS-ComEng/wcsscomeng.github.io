<?php
// Database credentials
$host = 'localhost';
$db   = 'comeng_server_database';
$user = 'root';
$pass = '6J(9u[0(m16^';

// Get JSON from POST
$data = json_decode(file_get_contents("php://input"), true);
$sensor_value = $data['sensor_value'] ?? null;

// Basic input validation
if ($sensor_value === null || !is_numeric($sensor_value)) {
    http_response_code(400);
    echo json_encode(["status" => "error", "message" => "Invalid input"]);
    exit;
}

try {
    $pdo = new PDO("mysql:host=$host;dbname=$db", $user, $pass);
    $stmt = $pdo->prepare("INSERT INTO sensor_readings (sensor_value) VALUES (:sensor_value)");
    $stmt->execute(['sensor_value' => $sensor_value]);

    echo json_encode(["status" => "success"]);
} catch (PDOException $e) {
    http_response_code(500);
    echo json_encode(["status" => "error", "message" => $e->getMessage()]);
}
