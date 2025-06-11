from flask import Flask, jsonify, request
from flask_cors import CORS
import mysql.connector
from mysql.connector import Error
import os
from dotenv import load_dotenv

# load environment variables
load_dotenv(dotenv_path="D:/arduinatedKohakuMain/wcsscomeng.github.io/server_tools/creds.env")

# useful code in POWERSHELL: py D:\arduinatedKohakuMain\wcsscomeng.github.io\server_tools\db_API.py
# browser http://10.191.28.229:5000/

# flask app setup
app = Flask(__name__)

# accept requests from listed addresses
CORS(app)

# Database configuration
DB_CONFIG = {
    "host": os.getenv("DB_HOST"),
    "user": os.getenv("DB_USER"),
    "password": os.getenv("DB_PASS"),
    "database": os.getenv("DB_NAME")
}

# ------------------------
# Route: /button-state
# ------------------------
@app.route("/button-state", methods=["GET"])
def get_button_state():
    try:
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
        if 'cursor' in locals(): cursor.close()
        if 'connection' in locals() and connection.is_connected(): connection.close()

# ------------------------
# Routes: /slider-values (GET & POST)
# ------------------------
@app.route("/slider-values", methods=["GET"])
def get_slider_values():
    try:
        connection = mysql.connector.connect(**DB_CONFIG)
        cursor = connection.cursor()
        cursor.execute("""
            SELECT slider1, slider2, colour, running
            FROM neopixel_info
            ORDER BY last_modified DESC
            LIMIT 1
        """)
        result = cursor.fetchone()
        if result:
            return jsonify({
                "slider1": result[0],
                "slider2": result[1],
                "colour": result[2],
                "running": bool(result[3])  # convert int to bool
            })
        else:
            return jsonify({"error": "No slider data found"}), 404
    except Error as e:
        print(f"MySQL error: {e}")
        return jsonify({"error": "Database error"}), 500
    finally:
        if 'cursor' in locals(): cursor.close()
        if 'connection' in locals() and connection.is_connected(): connection.close()


@app.route("/slider-values", methods=["POST"])
def post_slider_values():
    try:
        data = request.get_json()

        slider1 = int(data.get("slider1", 0))
        slider2 = int(data.get("slider2", 0))
        colour = data.get("colour", "#000000")  # default black
        running = 1 if data.get("running") else 0  # convert boolean to int

        connection = mysql.connector.connect(**DB_CONFIG)
        cursor = connection.cursor()
        cursor.execute("""
            INSERT INTO neopixel_info (slider1, slider2, colour, running, last_modified)
            VALUES (%s, %s, %s, %s, NOW())
        """, (slider1, slider2, colour, running))
        connection.commit()

        return jsonify({"message": "Slider values saved successfully"}), 201

    except Error as e:
        print(f"MySQL error: {e}")
        return jsonify({"error": "Database error"}), 500
    except Exception as e:
        print(f"App error: {e}")
        return jsonify({"error": "Bad request"}), 400
    finally:
        if 'cursor' in locals(): cursor.close()
        if 'connection' in locals() and connection.is_connected(): connection.close()


# ------------------------
# Routes: /contact-requests (GET & POST)
# ------------------------
@app.route("/contact-requests", methods=["GET"])
def get_contact_requests():
    try:
        connection = mysql.connector.connect(**DB_CONFIG)
        cursor = connection.cursor()
        cursor.execute("""
            SELECT first_name, last_name, grade, email, message
            FROM contact_requests
            ORDER BY last_modified DESC
            LIMIT 1
        """)
        result = cursor.fetchone()
        if result:
            return jsonify({
                "first_name": result[0],
                "last_name": result[1],
                "grade": result[2],
                "email": result[3],
                "message": result[4]
            })
        else:
            return jsonify({"error": "no contact request found"}), 404
    except Error as e:
        print(f"MySQL error: {e}")
        return jsonify({"error": "database error"}), 500
    finally:
        if 'cursor' in locals(): cursor.close()
        if 'connection' in locals() and connection.is_connected(): connection.close()

@app.route("/contact-requests", methods=["POST"])
def post_contact_request():
    try:
        data = request.get_json(force=True)
        
        connection = mysql.connector.connect(**DB_CONFIG)
        cursor = connection.cursor()
        cursor.execute("""
            INSERT INTO contact_requests (first_name, last_name, grade, email, message, last_modified)
            VALUES (%s, %s, %s, %s, %s, NOW())
        """, (
            data.get("first_name"),
            data.get("last_name"),
            data.get("grade"),
            data.get("email"),
            data.get("message")
        ))
        connection.commit()

        return jsonify({"message": "Contact request saved successfully!"}), 201
    except Error as e:
        print(f"MySQL error: {e}")
        return jsonify({"error": "Database error"}), 500
    except Exception as e:
        print(f"App error: {e}")
        return jsonify({"error": "Bad request"}), 400
    finally:
        if 'cursor' in locals(): cursor.close()
        if 'connection' in locals() and connection.is_connected(): connection.close()

# ------------------------
# Run the server
# ------------------------
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
