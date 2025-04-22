# encoding: utf-8
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import time
import six
import nixnet
from nixnet import constants
from nixnet import types

#定义一个全局变量，用于逐个转接板下载初始化代码
def static_vars(**kwargs):
    def decorate(func):
        for k in kwargs:
            setattr(func, k, kwargs[k])
        return func
    return decorate
    
@static_vars(canID = 0)
def setId(bridge_board_num):
    setId.canID = bridge_board_num
    #print('canID = %d' % setId.canID)


#发送函数
def WriteI2C(slave_address,register_address,data):
    interface1 = 'CAN1'
    

    with nixnet.FrameInStreamSession(interface1) as input_session:
        with nixnet.FrameOutStreamSession(interface1) as output_session:
                      
            input_session.intf.can_term = constants.CanTerm.ON
            output_session.intf.can_term = constants.CanTerm.ON

            input_session.intf.baud_rate = 500000
            output_session.intf.baud_rate = 500000
            input_session.start()
            
            payload_list = [0,0,0,0,0,0,0,0]
            payload_list[0] = 0x03
            payload_list[1] = 0x01
            payload_list[2] = slave_address
            payload_list[3] = register_address
            payload_list[4] = data
            payload_list[5] = 0
            payload_list[6] = 0
            payload_list[7] = setId.canID
            
            
            id = types.CanIdentifier(0x32)
            payload = bytearray(payload_list)
            frame = types.CanFrame(id, constants.FrameType.CAN_DATA, payload)

            frame.payload = payload
            output_session.frames.write([frame])
            #print('Sent frame with ID: {} payload: {}'.format(frame.identifier,
            #                                                     list(frame.payload)))
            #time.sleep(0.01)
            count = 1
            frames = input_session.frames.read(count)
           #for frame in frames:
           #    print('Received frame with ID: {} payload: {}'.format(frame.identifier,
           #                                                          list(six.iterbytes(frame.payload))))
           #    #print('payload[2] = %x'   %frame.payload[2])
           #read_data = frames[0].payload[2]
           #return  read_data

#接受函数
def ReadI2C(slave_address,register_address,len):
    interface1 = 'CAN1'
    
    with nixnet.FrameInStreamSession(interface1) as input_session:
        with nixnet.FrameOutStreamSession(interface1) as output_session:
                      
            input_session.intf.can_term = constants.CanTerm.ON
            output_session.intf.can_term = constants.CanTerm.ON

            input_session.intf.baud_rate = 500000
            output_session.intf.baud_rate = 500000
            input_session.start()
            
            payload_list = [0,0,0,0,0,0,0,0]
            payload_list[0] = 0x03
            payload_list[1] = 0x02
            payload_list[2] = slave_address
            payload_list[3] = register_address
            payload_list[4] = len
            payload_list[5] = 0
            payload_list[6] = 0
            payload_list[7] = setId.canID
            id = types.CanIdentifier(0x32)
            payload = bytearray(payload_list)
            frame = types.CanFrame(id, constants.FrameType.CAN_DATA, payload)
         
            frame.payload = payload
            output_session.frames.write([frame])
            #print('Sent frame with ID: {} payload: {}'.format(frame.identifier,
            #                                                      list(frame.payload)))
            time.sleep(1)

            count = 1
            frames = input_session.frames.read(count)
            for frame in frames:
                print('Received frame with ID: {} payload: {}'.format(frame.identifier,
                                                                     list(six.iterbytes(frame.payload))))
                #print('payload[2] = %d'  . frame.payload[2])
            read_data = frame.payload[2]
            return  ord(read_data)


def endI2C():
    interface1 = 'CAN1'
    
    with nixnet.FrameInStreamSession(interface1) as input_session:
        with nixnet.FrameOutStreamSession(interface1) as output_session:
                      
            input_session.intf.can_term = constants.CanTerm.ON
            output_session.intf.can_term = constants.CanTerm.ON

            input_session.intf.baud_rate = 500000
            output_session.intf.baud_rate = 500000
            input_session.start()
            
            payload_list = [0,0,0,0,0,0,0,0]
            payload_list[0] = 0x03
            payload_list[1] = 0x00
            payload_list[2] = 0X0
            payload_list[3] = 0X0
            payload_list[4] = 0X0
            payload_list[5] = 0
            payload_list[6] = 0
            payload_list[7] = setId.canID
            
            id = types.CanIdentifier(0x32)
            payload = bytearray(payload_list)
            frame = types.CanFrame(id, constants.FrameType.CAN_DATA, payload)
         
            frame.payload = payload
            output_session.frames.write([frame])
            #print('Sent frame with ID: {} payload: {}'.format(frame.identifier,
            #                                                      list(frame.payload)))
            time.sleep(0.1)

            count = 1
            frames = input_session.frames.read(count)
            #time.sleep(1)
            #output_session.intf.can_term = constants.CanTerm.OFF
            #input_session.intf.can_term = constants.CanTerm.OFF   
    with nixnet.FrameInStreamSession(interface1) as input_session:
        with nixnet.FrameOutStreamSession(interface1) as output_session:
                      
            input_session.intf.can_term = constants.CanTerm.OFF
            output_session.intf.can_term = constants.CanTerm.OFF            

            
def main():
    interface1 = 'CAN1'
    interface2 = 'CAN2'

    with nixnet.FrameInStreamSession(interface1) as input_session:
        with nixnet.FrameOutStreamSession(interface1) as output_session:
            terminated_cable = six.moves.input('Are you using a terminated cable (Y or N)? ')
            if terminated_cable.lower() == "y":
                input_session.intf.can_term = constants.CanTerm.ON
                output_session.intf.can_term = constants.CanTerm.OFF
            elif terminated_cable.lower() == "n":
                input_session.intf.can_term = constants.CanTerm.ON
                output_session.intf.can_term = constants.CanTerm.ON
            else:
                print("Unrecognised input ({}), assuming 'n'".format(terminated_cable))
                input_session.intf.can_term = constants.CanTerm.ON
                output_session.intf.can_term = constants.CanTerm.ON

            input_session.intf.baud_rate = 500000
            output_session.intf.baud_rate = 500000

            # Start the input session manually to make sure that the first
            # frame value sent before the initial read will be received.
            input_session.start()
            

            payload_list = [0,0,0,0,0,0,0,0]
            payload_list[0] = 0x03
            payload_list[1] = 0x01
            payload_list[2] = 0x18
            payload_list[3] = 0x28
            payload_list[4] = 0x95
            id = types.CanIdentifier(0x32)
            payload = bytearray(payload_list)
            frame = types.CanFrame(id, constants.FrameType.CAN_DATA, payload)

            print('The same values should be received. Press q to quit')
            i = 0
            while True:
                for index, byte in enumerate(payload):
                    payload[index] = byte + i

                frame.payload = payload
                output_session.frames.write([frame])
                print('Sent frame with ID: {} payload: {}'.format(frame.identifier,
                                                                  list(frame.payload)))

                # Wait 1 s and then read the received values.
                # They should be the same as the ones sent.
                time.sleep(0.1)

                count = 1
                frames = input_session.frames.read(count)
                for frame in frames:
                    print('Received frame with ID: {} payload: {}'.format(frame.identifier,
                                                                          list(six.iterbytes(frame.payload))))
                    print('payload[5] = %x'   %frame.payload[5])
                i += 1
                if max(payload) + i > 0xFF:
                    i = 0

                inp = six.moves.input('Hit enter to continue (q to quit): ')
                if inp.lower() == 'q':
                    break

            print('Data acquisition stopped.')


if __name__ == '__main__':
    main()
