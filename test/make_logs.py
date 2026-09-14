import random
import datetime

IP_POOL = [
    f"{random.randint(1, 255)}.{random.randint(0, 255)}.{random.randint(0, 255)}.{random.randint(1, 255)}" 
    for _ in range(15)
]
def generate_log_entry():
    """Generate a single line formatted as an Apache/Nginx Combined Log entry."""
    methods = ["GET", "GET", "GET", "POST", "PUT", "DELETE"]
    endpoints = ["/", "/index.html", "/login", "/api/v1/users", "/images/logo.png", "/dashboard", "/style.css"]
    protocols = ["HTTP/1.1", "HTTP/2.0"]
    status_codes = [200, 200, 200, 201, 301, 401, 403, 404, 500]
    user_agents = [
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/16.1 Safari/605.1.15",
        "Mozilla/5.0 (iPhone; CPU iPhone OS 16_5 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/16.5 Mobile/15E148 Safari/604.1",
        "curl/7.81.0",
        "PostmanRuntime/7.32.2"
    ]

    ip = random.choice(IP_POOL)
    
    days_ago = random.randint(0, 7)
    minutes_ago = random.randint(0, 1440)
    log_date = datetime.datetime.now() - datetime.timedelta(days=days_ago, minutes=minutes_ago)
    date_str = log_date.strftime("%d/%b/%Y:%H:%M:%S +0000")
    
    method = random.choice(methods)
    endpoint = random.choice(endpoints)
    protocol = random.choice(protocols)
    status = random.choice(status_codes)
    bytes_sent = random.randint(100, 10500)
    referer = "-"
    user_agent = random.choice(user_agents)

    log_line = f'[{date_str}] {method} {endpoint} {protocol} {ip} {status} {bytes_sent} "{referer}" "{user_agent}"'
    
    return log_line

def create_log_file(filename="access.log", num_lines=100):
    """Write the generated entries to a file."""
    with open(filename, 'w') as f:
        for _ in range(num_lines):
            f.write(generate_log_entry() + "\n")
    print(f"Successfully generated {num_lines} lines in '{filename}'")

if __name__ == "__main__":
    create_log_file("access.log", 100)