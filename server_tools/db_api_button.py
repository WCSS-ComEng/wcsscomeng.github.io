from flask import Flask, jsonify
import mysql.connector
from mysql.connector import Error
from flask_cors import CORS
import os

# ensures safety of database credentials
from dotenv import load_dotenv
load_dotenv()

app = Flask(__name__)
CORS(app)

# useful code in POWERSHELL: py C:\Users\gsorg1\Documents\GitHub\wcsscomeng.github.io\server_tools\db_api_button.py
# browser http://10.191.28.229:5000/button-state

# database connection config
DB_CONFIG = {
    "host": os.getenv("DB_HOST"),
    "user": os.getenv("DB_USER"),
    "password": os.getenv("DB_PASS"),
    "database": os.getenv("DB_NAME")
}

@app.route("/button-state", methods=["GET"])
def get_button_state():
    try:
        # connect to MySQL
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

# run the Flask server on all interfaces (so it can be accessed from other devices)
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
