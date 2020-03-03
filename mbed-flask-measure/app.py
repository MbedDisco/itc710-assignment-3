from flask import Flask, request, jsonify
import os
import psycopg2

app = Flask(__name__)
app.config["TEMPLATES_AUTO_RELOAD"] = True
DATABASE_URL = os.environ['DATABASE_URL']

@app.route('/update', methods=["GET"])
def update():
    temp = request.args.get('temp')
    hum = request.args.get('hum')
    conn = psycopg2.connect(DATABASE_URL, sslmode='require')
    c = conn.cursor()
    postgres_insert_query = """ INSERT INTO records (temp, hum) VALUES (%s,%s)""" 
    record_to_insert = (temp,hum)
    c.execute(postgres_insert_query, record_to_insert)
    conn.commit()
    conn.close()
    resp = {'status':'OK'}
    return jsonify(resp)

@app.route('/query', methods=["GET"])
def query():
    name = request.args.get('name')
    conn = psycopg2.connect(DATABASE_URL, sslmode='require')
    c = conn.cursor()
    c.execute('SELECT * FROM records')
    records = c.fetchall()
    results = []
    for r in records:
        results.append({'timestamp':r[1], 'temp':r[2], 'hum':r[3]})
    conn.commit()
    conn.close()
    resp = {'status':'OK', 'results':results}
    return jsonify(resp)

if __name__ == '__main__':
    conn = psycopg2.connect(DATABASE_URL, sslmode='require')
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS records
             (_id SERIAL UNIQUE,
             timestamp timestamp DEFAULT CURRENT_TIMESTAMP,
             temp TEXT NOT NULL,hum TEXT NOT NULL);''')
    conn.commit()
    conn.close()
    port = int(os.environ.get('PORT', 5000))
    app.run(debug=True,host='0.0.0.0', port=port)
