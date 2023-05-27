import socket
import mediapipe as mp
import cv2
import json

# 소켓 서버 설정
HOST = '127.0.0.1'
PORT = 9999

# 미디어파이프 설정
mp_drawing = mp.solutions.drawing_utils
mp_pose = mp.solutions.pose

def main():
    # 소켓 생성
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind((HOST, PORT))
    server_socket.listen()

    print(f"Server listening on {HOST}:{PORT}")

    # 클라이언트 연결 대기
    client_socket, client_address = server_socket.accept()
    print(f"Connected by {client_address}")

    # 미디어파이프 포즈 실행
    cap = cv2.VideoCapture(0)
    while True:
        with mp_pose.Pose(min_detection_confidence=0.5, min_tracking_confidence=0.5) as pose:
            while cap.isOpened():
                ret, frame = cap.read()
                if not ret:
                    break

                # 포즈 인식
                image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
                image.flags.writeable = False
                results = pose.process(image)

                # 랜드마크 정보 전송
                if results.pose_landmarks:
                    landmarks = results.pose_landmarks.landmark
                    targetdata = {10, 11, 12, 13, 14, 15, 16, 23, 24, 25, 26 ,27, 28, 29, 30, 31, 32}
                    landmark_data = [{'Num' : l,'x': landmarks[l].x, 'y': landmarks[l].y, 'z': landmarks[l].z, 'visibility': landmarks[l].visibility} for l in targetdata]
                    #landmark_data = [{'x': landmarks[10].x, 'y': landmarks[10].y, 'z': landmarks[10].z, 'visibility': landmarks[10].visibility}]
                    client_socket.sendall(json.dumps(landmark_data).encode('utf-8'))

                # 웹캠 화면 출력
                image.flags.writeable = True
                image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
                mp_drawing.draw_landmarks(image, results.pose_landmarks, mp_pose.POSE_CONNECTIONS)
                cv2.imshow('MediaPipe Pose', image)
                if cv2.waitKey(5) & 0xFF == 27:
                    break

        cap.release()
        cv2.destroyAllWindows()

    # client_socket.close()
    # server_socket.close()

if __name__ == "__main__":
            main()
