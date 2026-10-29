import subprocess
import os
from flask import Flask, request, jsonify, send_from_directory

app = Flask(__name__, static_folder='.')

@app.route('/')
def index():
    return send_from_directory('.', 'index.html')

@app.route('/compile', methods=['POST'])
def compile_code():
    data = request.get_json()
    code = data.get('code', '')

    # Write submitted code to temporary input file
    with open('temp_input.txt', 'w') as f:
        f.write(code)

    try:
        # Run compiler binary with redirected input file
        process = subprocess.Popen(
            ['./compiler'],
            stdin=open('temp_input.txt', 'r'),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        stdout, stderr = process.communicate()

        if process.returncode != 0 or stderr:
            return jsonify({
                'success': False,
                'error': stderr if stderr else "Syntax error during compilation."
            })

        return jsonify({
            'success': True,
            'output': stdout
        })

    except Exception as e:
        return jsonify({'success': False, 'error': str(e)})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)