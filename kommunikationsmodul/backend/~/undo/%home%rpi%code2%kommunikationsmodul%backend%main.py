Vim�UnDo� �re6_0����s�w]��SO5��E�61.  -           !      B       B   B   B    gXrG    _�                       &    ����                                                                                                                                                                                                                                                                                                                                                             gX[�    �      &      &                        addAdjacent();5��      %                  AK         5�_�                   �       ����                                                                                                                                                                                                                                                                                                                                                             gXf*     �  �  �  &          if not driverReady:5��    �                   �2         �    �                     �2         �    �                    �2         �    �                     �2         �    �                    �2         5�_�                   �        ����                                                                                                                                                                                                                                                                                                                                                             gXf-     �  �  �  )       5��    �                     �2         5�_�                   �       ����                                                                                                                                                                                                                                                                                                                                                             gXf=     �  �  �  (              �  �  �  )           �  �  �  )          counter: int = 05��    �                     �2         �    �                     �2         �    �                    �2         5�_�                   �       ����                                                                                                                                                                                                                                                                                                                                                             gXfE     �  �  �  '      "    uart_send(driver_ttyUSB, data)5��    �                   �2         �    �                     �2         �    �                    �2         5�_�                   �       ����                                                                                                                                                                                                                                                                                                                                                             gXff     �  �  �  )      !    if not DEBUG_STANDALONE_MODE:5��    �                    3         �    �                     !3         �    �                    !3         5�_�                   �        ����                                                                                                                                                                                                                                                                                                                                                             gXfi     �  �  �  +       5��    �                     !3         5�_�      	             �   E    ����                                                                                                                                                                                                                                                                                                                                                             gXf�     �  �  �  +      F        while int.from_bytes(uart_recv(driver_ttyUSB), 'big') != 0x0A:5��    �  E                  �3         5�_�      B           	  �   *    ����                                                                                                                                                                                                                                                                                                                                                             gXf�    �  �  �  +      *            uart_send(driver_ttyUSB, data)5��    �  *                 �3         �    �                    �3         5�_�   	       
       B  �        ����                                                                                                                                                                                                                                                                                                                           �         �          V       gXrF    �  �  �  +    �  �  �  +    �  �  �          K        client_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)5��    �                     �A  L       �    �                     �A      �   5�_�   	          B   
  ~   $    ����                                                                                                                                                                                                                                                                                                                                                             gXj	     �  }    ,      4def sendCommandWithRetry(data: bytes, conn) -> None:5��    }  $                  �2         5�_�   
                �   3    ����                                                                                                                                                                                                                                                                                                                                                             gXj     �  �  �  ,      :        sendCommandWithRetry((5).to_bytes(1, 'big'), conn)5��    �  3                  6         5�_�                  �   3    ����                                                                                                                                                                                                                                                                                                                                                             gXj     �  �  �  ,      :        sendCommandWithRetry((1).to_bytes(1, 'big'), conn)5��    �  3                  M6         5�_�                   �   3    ����                                                                                                                                                                                                                                                                                                                                                             gXj     �  �  �  ,      :        sendCommandWithRetry((6).to_bytes(1, 'big'), conn)5��    �  3                  G9         �    �  3                 G9        5�_�                   �   3    ����                                                                                                                                                                                                                                                                                                                                                             gXj     �  �  �  ,      :        sendCommandWithRetry((2).to_bytes(1, 'big'), conn)5��    �  3                  �9         �    �  3                 �9        5�_�                   �   3    ����                                                                                                                                                                                                                                                                                                                                                             gXj     �  �  �  ,      :        sendCommandWithRetry((7).to_bytes(1, 'big'), conn)5��    �  3                  �<         5�_�                   �   3    ����                                                                                                                                                                                                                                                                                                                                                             gXj     �  �  �  ,      :        sendCommandWithRetry((3).to_bytes(1, 'big'), conn)5��    �  3                  �<         5�_�                   �   3    ����                                                                                                                                                                                                                                                                                                                                                             gXj     �  �  �  ,      :        sendCommandWithRetry((8).to_bytes(1, 'big'), conn)5��    �  3                  !>         �    �  4                 ">        �    �  4                 ">        5�_�                   �   3    ����                                                                                                                                                                                                                                                                                                                                                             gXj"     �  �  �  ,      :        sendCommandWithRetry((4).to_bytes(1, 'big'), conn)5��    �  3                  f>         5�_�                      7    ����                                                                                                                                                                                                                                                                                                                                                             gXj%     �      ,      >        sendCommandWithRetry(command.to_bytes(1, 'big'), conn)5��      7                  Y         5�_�                   	   ,    ����                                                                                                                                                                                                                                                                                                                                                             gXj*     �    
  ,      <def moveToDirection(nextDirection: Direction, conn) -> None:5��      ,                           5�_�                   �       ����                                                                                                                                                                                                                                                                                                                                                             gXj3     �  �  �  ,      ,def send_forward(short: bool, conn) -> None:5��    �                    '5         5�_�                   �       ����                                                                                                                                                                                                                                                                                                                                                             gXj8     �  �  �  ,      -def send_backward(short: bool, conn) -> None:5��    �                    l8         5�_�                   �       ����                                                                                                                                                                                                                                                                                                                                                             gXj<     �  �  �  ,      *def turn_right(short: bool, conn) -> None:5��    �                    �;         5�_�                   �       ����                                                                                                                                                                                                                                                                                                                                                             gXj@    �  �  �  ,      )def turn_left(short: bool, conn) -> None:5��    �                    O=         �    �                   O=        �    �                   O=        �    �                   O=        5�_�                   �        ����                                                                                                                                                                                                                                                                                                                                    �          V       gXjk     �  �  �  ,      !        send_forward(False, conn)�  �  �          -                            send_stop(, conn)�  �  �          5                            send_forward(False, conn)�  �  �          6                            send_backward(False, conn)�  �             4                            send_forward(True, conn)�              5                            send_backward(True, conn)5��    �                   kC        �    �                   �D        �    �                   �E        �    �                   �E        �    �                   \G        �                       �G        5�_�                   �   ,    ����                                                                                                                                                                                                                                                                                                                                    �          V       gXjt     �  �  �  ,      3                            turn_right(False, conn)5��    �  ,                  �F         5�_�                   �   +    ����                                                                                                                                                                                                                                                                                                                                    �          V       gXjv     �  �  �  ,      2                            turn_left(False, conn)5��    �  +                  �F         5�_�                      +    ����                                                                                                                                                                                                                                                                                                                           �   '      �   &       v   &    gXjx     �      ,      2                            turn_right(True, conn)5��      +                  lH         5�_�                       *    ����                                                                                                                                                                                                                                                                                                                                                             gXjy    �    	  ,      1                            turn_left(True, conn)5��      *                  �H         5�_�      !             �   &    ����                                                                                                                                                                                                                                                                                                                           �   '      �   &       v   &    gXj�     �  �  �  ,      +                            send_stop(conn)5��    �  &                  �D         5�_�       #           !  �   &    ����                                                                                                                                                                                                                                                                                                                           �   .      �   &       v   &    gXj�    �  �  �  ,      '                            send_stop()5��    �  &                  �D         5�_�   !   $   "       #  �        ����                                                                                                                                                                                                                                                                                                                                                             gXj�     �  �  �        5��    �                     �C  3       5�_�   #   %           $  �        ����                                                                                                                                                                                                                                                                                                                                                             gXj�     �  �  �  +    �  �  �  +      2            conn, address = client_socket.accept()5��    �                     cC      3   5�_�   $   &           %  �       ����                                                                                                                                                                                                                                                                                                                                                             gXj�    �  �  �  ,      .        conn, address = client_socket.accept()5��    �                    cC        5�_�   %   '           &  �       ����                                                                                                                                                                                                                                                                                                                                                             gXk     �  �  �  ,       5��    �                     33         �    �                     33         5�_�   &   (           '  �        ����                                                                                                                                                                                                                                                                                                                                                             gXk     �  �  �  -    �  �  �  -      ;                sendSensorDataToInterface(conn, sensorData)5��    �                     43      <   5�_�   '   )           (  �       ����                                                                                                                                                                                                                                                                                                                                                             gXk     �  �  �          ;                sendSensorDataToInterface(conn, sensorData)5�5�_�   (   *           )  �       ����                                                                                                                                                                                                                                                                                                                                                             gXk	     �  �  �  .      7            sendSensorDataToInterface(conn, sensorData)5��    �                    43        5�_�   )   +           *  �       ����                                                                                                                                                                                                                                                                                                                                                             gXk	     �  �  �  .      3        sendSensorDataToInterface(conn, sensorData)5��    �                    43        5�_�   *   ,           +  �       ����                                                                                                                                                                                                                                                                                                                                                             gXk
     �  �  �  .      /    sendSensorDataToInterface(conn, sensorData)5��    �                    43        5�_�   +   -           ,  �   $    ����                                                                                                                                                                                                                                                                                                                                                             gXk     �  �  �  .      1    sendSensorDataToInterface(conn, [1, 1, 1, 1])5��    �  $       
          X3  
      5�_�   ,   .           -  �   &    ����                                                                                                                                                                                                                                                                                                                           �   .      �   &       v   &    gXk     �  �  �  .      (    sendSensorDataToInterface(conn, [1])5��    �  &       	           Z3  	       5�_�   -   /           .  �   &    ����                                                                                                                                                                                                                                                                                                                           �   .      �   &       v   &    gXk    �  �  �  .      *    sendSensorDataToInterface(conn, [420])5��    �  %                 Y3        5�_�   .   0           /  �       ����                                                                                                                                                                                                                                                                                                                           �   .      �   &       v   &    gXk%     �  �  �  .    �  �  �  .      *    sendSensorDataToInterface(conn, [420])5��    �                     M4      +   5�_�   /   1           0  �   %    ����                                                                                                                                                                                                                                                                                                                           �   J      �   J       V���    gXk'    �  �  �  /      )    sendSensorDataToInterface(conn, [69])5��    �  %                 r4        5�_�   0   2           1   g       ����                                                                                                                                                                                                                                                                                                                                                             gXlL    �   g   h  /              �   g   i  0              sys.exit(2)5��    g                      �      	   �    g                     �         5�_�   1   3           2   h       ����                                                                                                                                                                                                                                                                                                                                                             gXlU    �   g   i  0              return -15��    g                    �        �    g                    	        5�_�   2   4           3     #    ����                                                                                                                                                                                                                                                                                                                                                             gXl`     �      0                              �      1      #                        sys.exit(2)5��                         LJ         �                        dJ         5�_�   3   5           4         ����                                                                                                                                                                                                                                                                                                                                                             gXlc    �            5��                         pJ         5�_�   4   6           5  �   V    ����                                                                                                                                                                                                                                                                                                                                                             gXo}     �  �  �  0      W        while int.from_bytes(uart_recv(driver_ttyUSB), 'big') != 0x0A and counter < 99:5��    �  V                  �3         5�_�   5   7           6  �       ����                                                                                                                                                                                                                                                                                                                                                             gXo~    �  �  �  0                  �  �  �  1                  time.sleep(0.01)5��    �                     D4         �    �                    P4         �    �                   P4        �    �                   ]4        5�_�   6   8           7  �       ����                                                                                                                                                                                                                                                                                                                                                             gXo�    �  �  �        5��    �                     D4         5�_�   7   9           8  �       ����                                                                                                                                                                                                                                                                                                                           �   J      �   J       V���    gXp     �  �  �  0    �  �  �  0      K        client_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)5��    �                     �B      L   5�_�   8   :           9  �   2    ����                                                                                                                                                                                                                                                                                                                           �   J      �   J       V���    gXp$     �  �  �  1      X        client_socket.setsockopt(socket.SOL_SOCKET | socket.TCP, socket.SO_REUSEADDR, 1)5��    �  2                  �B         5�_�   9   ;           :  �   ?    ����                                                                                                                                                                                                                                                                                                                           �   J      �   J       V���    gXp.    �  �  �  1      `        client_socket.setsockopt(socket.SOL_SOCKET | socket.TCP_NODELAY, socket.SO_REUSEADDR, 1)5��    �  ?                  �B         5�_�   :   <           ;  �   F    ����                                                                                                                                                                                                                                                                                                                           �   2      �   F       v   F    gXpM    �  �  �        5��    �                     �B  L       5�_�   ;   =           <  �       ����                                                                                                                                                                                                                                                                                                                                                             gXp^     �  �  �  0    �  �  �  0      `        client_socket.setsockopt(socket.SOL_SOCKET | socket.TCP_NODELAY, socket.SO_REUSEADDR, 1)5��    �                     �B      a   5�_�   <   >           =  �   2    ����                                                                                                                                                                                                                                                                                                                           �   !      �   !       V   !    gXpb     �  �  �  1      K        client_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)5��    �  2                  �B         5�_�   =   ?           >  �   !    ����                                                                                                                                                                                                                                                                                                                                                             gXpe    �  �  �  1      L        client_socket.setsockopt(socket.TCP_NODELAY, socket.SO_REUSEADDR, 1)5��    �  !                  C         5�_�   >   @           ?  �       ����                                                                                                                                                                                                                                                                                                                                                             gXpk    �  �  �  1      N        # client_socket.setsockopt(socket.TCP_NODELAY, socket.SO_REUSEADDR, 1)5��    �                    �B         5�_�   ?   A           @  �       ����                                                                                                                                                                                                                                                                                                                                                             gXpt     �  �  �  1      M         client_socket.setsockopt(socket.TCP_NODELAY, socket.SO_REUSEADDR, 1)5��    �                    �B         5�_�   @               A  �       ����                                                                                                                                                                                                                                                                                                                                                             gXpt    �  �  �  1      L        client_socket.setsockopt(socket.TCP_NODELAY, socket.SO_REUSEADDR, 1)5��    �                    �B         5�_�   !           #   "  �        ����                                                                                                                                                                                                                                                                                                                                                             gXj�     �  �  �        5��    �                     cC  "       5�_�                    �   &    ����                                                                                                                                                                                                                                                                                                                           �   '      �   &       v   &    gXj�     �  �  �  ,      +                            send_stop(conn)5��    �  &                  �D         5�_�                   �   	    ����                                                                                                                                                                                                                                                                                                                                                             gXj     �  �  �  ,      :        s, connendCommandWithRetry((1).to_bytes(1, 'big'))5��    �  	                  #6         5��