from flask import Flask, jsonify, request
import mysql.connector
from mysql.connector import Error
from flask_cors import CORS
import os

# ensures safety of database credentials
from dotenv import load_dotenv
load_dotenv(dotenv_path="C:/Users/gsorg1/Documents/GitHub/wcsscomeng.github.io/server_tools/creds.env")

app = Flask(__name__)
CORS(app)

# useful code in POWERSHELL: py C:\Users\gsorg1\Documents\GitHub\wcsscomeng.github.io\server_tools\db_contacts.py
# browser http://10.191.28.229:5000/contact-requests
# database connection config

DB_CONFIG = {
    "host": os.getenv("DB_HOST"),
    "user": os.getenv("DB_USER"),
    "password": os.getenv("DB_PASS"),
    "database": os.getenv("DB_NAME")
}

#####################
### GET DB VALUES ###
#####################

@app.route("/contact-requests", methods=["GET"])
def get_slider_values():
    try:
        # connect to MySQL
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
            contact_requests = {
                "first_name": result[0],
                "last_name": result[1],
                "grade": result[2],
                "email": result[3],
                "message": result[4]
            }
            return jsonify(contact_requests)
        else:
            return jsonify({"error": "No slider data found"}), 404
    except Error as e:
        print(f"MySQL error: {e}")
        return jsonify({"error": "Database error"}), 500
    
    # closes connections, otherwise prevents data leaks
    finally:
        if 'cursor' in locals():
            cursor.close()
        if 'connection' in locals() and connection.is_connected():
            connection.close()

######################
### POST DB VALUES ###
######################

@app.route("/contact-requests", methods=["POST"])
def post_contact_request():
    try:
        data = request.get_json()
        first_name = data.get("first_name")
        last_name = data.get("last_name")
        grade = data.get("grade")
        email = data.get("email")
        message = data.get("message")

        # Insert data into MySQL database
        connection = mysql.connector.connect(**DB_CONFIG)
        cursor = connection.cursor()
        query = """
            INSERT INTO contact_requests (first_name, last_name, grade, email, message)
            VALUES (%s, %s, %s, %s, %s)
        """
        cursor.execute(query, (first_name, last_name, grade, email, message))
        connection.commit()

        return jsonify({"message": "Contact request saved successfully!"}), 201
    except Error as e:
        print(f"MySQL error: {e}")
        return jsonify({"error": "Database error"}), 500
    except Exception as e:
        print(f"App error: {e}")
        return jsonify({"error": "Bad request"}), 400
    finally:
        if 'cursor' in locals():
            cursor.close()
        if 'connection' in locals() and connection.is_connected():
            connection.close()

# run the Flask server on all interfaces (so it can be accessed from other devices)
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
