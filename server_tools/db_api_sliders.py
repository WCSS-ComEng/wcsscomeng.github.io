from flask import Flask, jsonify, request
import mysql.connector
from mysql.connector import Error
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

# useful code in POWERSHELL: py C:\Users\gsorg1\Documents\GitHub\wcsscomeng.github.io\server_tools\db_api_sliders.py
# browser http://10.191.28.229:5000/slider-values

# database connection config
DB_CONFIG = {
    "host": "localhost",
    "user": "API_CONNECTOR",
    "password": "endpointserverworkbench",
    "database": "comeng_server_database"
}

#####################
### GET DB VALUES ###
#####################

@app.route("/slider-values", methods=["GET"])
def get_slider_values():
    try:
        # connect to MySQL
        connection = mysql.connector.connect(**DB_CONFIG)
        cursor = connection.cursor()
        cursor.execute("""
            SELECT slider1, slider2, slider3
            FROM slider_values
            ORDER BY last_modified DESC
            LIMIT 1
        """)
        result = cursor.fetchone()
        if result:
            slider_values = {
                "slider1": result[0],
                "slider2": result[1],
                "slider3": result[2]
            }
            return jsonify(slider_values)
        else:
            return jsonify({"error": "No slider data found"}), 404
    except Error as e:
        print(f"MySQL error: {e}")
        return jsonify({"error": "Database error"}), 500
    finally:
        if 'cursor' in locals():
            cursor.close()
        if 'connection' in locals() and connection.is_connected():
            connection.close()

######################
### POST DB VALUES ###
######################

@app.route("/slider-values", methods=["POST"])
def post_slider_values():
    try:
        data = request.get_json()

        slider1 = int(data.get("slider1", 0))
        slider2 = int(data.get("slider2", 0))
        slider3 = int(data.get("slider3", 0))

        connection = mysql.connector.connect(**DB_CONFIG)
        cursor = connection.cursor()

        query = """
            INSERT INTO slider_values (slider1, slider2, slider3, last_modified)
            VALUES (%s, %s, %s, NOW())
        """
        cursor.execute(query, (slider1, slider2, slider3))
        connection.commit()

        return jsonify({"message": "Slider values saved successfully"}), 201

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
