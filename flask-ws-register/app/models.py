from datetime import datetime
from app import db


class Temperature(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    temperature = db.Column(db.Float())
    timestamp = db.Column(db.DateTime, index=True, default=datetime.utcnow)

    @property
    def serialize(self):
        return {
            'temperature': self.temperature,
            'tiimestamp': self.timestamp,
        }

    def __repr__(self):
        return '<Temperature {}, timestamp {}>'.format(self.temperature, self.timestamp)


class Humidity(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    humidity = db.Column(db.Float())
    timestamp = db.Column(db.DateTime, index=True, default=datetime.utcnow)

    @property
    def serialize(self):
        return {
            'humidity': self.humidity,
            'tiimestamp': self.timestamp,
        }

    def __repr__(self):
        return '<Humidity {}, timestamp {}>'.format(self.humidity, self.timestamp)
