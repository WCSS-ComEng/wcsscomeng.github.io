from flask import Flask, jsonify
import mysql.connector
from mysql.connector import Error

app = Flask(__name__)

# Database connection config
DB_CONFIG = {
    "host": "localhost",              # MySQL is local to this PC
    "user": "root",                   # Use a non-root user if possible
    "password": "dolphindatabaseworkbench",       # Replace with your secure password
    "database": "comeng_server_database"
}

@app.route("/button-state", methods=["GET"])
def get_button_state():
    try:
        # Connect to MySQL
        connection = mysql.connector.connect(**DB_CONFIG)
        cursor = connection.cursor()
        cursor.execute("SELECT state FROM button_values ORDER BY last_modified DESC LIMIT 1")
        result = cursor.fetchone()
        state = result[0] if result else 0
        return jsonify({"state": state})
    except Error as e:
        print(f"MySQL error: {e}")
        return jsonify({"error": "Database error"}), 500
    finally:
        if 'cursor' in locals():
            cursor.close()
        if 'connection' in locals() and connection.is_connected():
            connection.close()

# Run the Flask server on all interfaces (so it can be accessed from other devices)
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
