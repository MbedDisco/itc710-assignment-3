import json

from flask import render_template, request, flash, redirect, url_for, jsonify, abort
from app import app, db
from app.models import Humidity, Temperature


@app.route("/")
def index():
    response = {
        "sensors": {
            "humidity": url_for("query", sensor_type="humidity", _external=True),
            "temperature": url_for("query", sensor_type="temperature", _external=True),
        }
    }
    return jsonify(response)


@app.route("/temperature", methods=["GET", "POST"])
def temperature():
    if request.method == "POST":
        if request.headers.get("X-API-KEY") != app.config["API_KEY"]:
            abort(401)
        if not request.json or not "data" in request.json:
            abort(400)
        else:
            if not request.json.get("data").get("temperature"):
                abort(404)
            t = Temperature(temperature=request.json.get("data")["temperature"])
            db.session.add(t)
            db.session.commit()
            return jsonify({"status": "ok"})
    temperatures = Temperature.query.all()
    return jsonify(temperature_data=[i.serialize for i in temperatures])


@app.route("/humidity", methods=["GET", "POST"])
def humidity():
    if request.method == "POST":
        if request.headers.get("X-API-KEY") != app.config["API_KEY"]:
            abort(401)
        if not request.json or not "data" in request.json:
            abort(400)
        else:
            if not request.json.get("data").get("humidity"):
                abort(404)
            h = Humidity(humidity=request.json.get("data")["humidity"])
            db.session.add(h)
            db.session.commit()
            return jsonify({"status": "ok"})
    humidity = Humidity.query.all()

    return jsonify(humidity_data=[i.serialize for i in humidity])


@app.route("/update/<sensor_type>", methods=["POST"])
def update(sensor_type):
    if sensor_type not in ("humidity", "temperature"):
        abort(404)

    if request.headers.get("X-API-KEY") != app.config["API_KEY"]:
        abort(401)

    if not request.json or not "data" in request.json:
        abort(400)

    if sensor_type == "humidity":
        if not request.json.get("data").get("humidity"):
            abort(404)
        h = Humidity(humidity=request.json.get("data")["humidity"])
        db.session.add(h)
        db.session.commit()
    else:
        if not request.json.get("data").get("temperature"):
            abort(404)
        t = Temperature(temperature=request.json.get("data")["temperature"])
        db.session.add(t)
        db.session.commit()

    return jsonify({"status": "ok"})


@app.route("/query/<sensor_type>", methods=["GET"])
def query(sensor_type):

    if sensor_type == "humidity":
        humidity = Humidity.query.all()
        return jsonify(humidity_data=[i.serialize for i in humidity])
    else:
        temperatures = Temperature.query.all()
        return jsonify(temperature_data=[i.serialize for i in temperatures])
