
import http.server
import os
import ssl
import sys

def start_https_server(ota_image_dir: str, server_ip: str, server_port: int, server_file: str = None, key_file: str = None) -> None:
    os.chdir(ota_image_dir)
   
    httpd = http.server.HTTPServer((server_ip, server_port), http.server.SimpleHTTPRequestHandler)

    httpd.socket = ssl.wrap_socket(httpd.socket,
                                   keyfile=key_file,
                                   certfile=server_file, server_side=True)
    httpd.serve_forever()



if __name__ == '__main__':
    if sys.argv[2:]:    # if two or more arguments provided:
        # Usage: pytest_simple_ota.py <image_dir> <server_port> [cert_di>]
        this_dir = os.path.dirname(os.path.realpath(__file__))
        bin_dir = os.path.join(this_dir, sys.argv[1])
        port = int(sys.argv[2])
        cert_dir = bin_dir if not sys.argv[3:] else os.path.join(this_dir, sys.argv[3])  # optional argument
        print('Starting HTTPS server at "https://:{}"'.format(port))
        start_https_server(bin_dir, '', port,
                           server_file=os.path.join(cert_dir, 'ca_cert.pem'),
                           key_file=os.path.join(cert_dir, 'ca_key.pem'))
