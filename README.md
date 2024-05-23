### IMU and Kinect
## 1. IMU 센서 두개 시각화 & SVD 활용해서 3D 포인트랑 2D 포인트 동시에 출력

두개의 imu 센서를 각기 시각화 한다
3D 공간에 출력하고, SVD를 통해서 2D좌표로 변환된것을 출력시키기
사용된 imu는 총 2개
C++, MFC기반으로 작성하였으며, 데이터 처리를 위해 서버와 클라이언트로 구성함
MFC_opengl_clinet, MFC_Server두 개 다운받아서 사용해야 사용 가능함

<img width="80%" src="https://github.com/LEEYUN-JU/IMU_and_Kinect/assets/55966445/cb0a1009-81a9-44b3-b9e6-a97f19605d92.gif"/>

---

## 2. IMU 센서랑 Azure Kinect 동시에 사용하는 프로그램

Server랑 ConsoleApplication 1을 사용한다.
Console 프로그램이다.

## 3. IMU 센서 python에서 시각화 하기

Python-Kinect파일 다운로드 후에 사용 가능

<img width="80%" src="https://github.com/LEEYUN-JU/IMU_and_Kinect/assets/55966445/d30d398f-2693-496e-b2da-4fcf42885180.gif"/>
