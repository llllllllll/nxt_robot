-- Joe Jevnik
-- 23.10.2013
-- NXT Robot testing.

import Robotics.NXT
import Data.Word
import System.IO
import Control.Monad


-- Represents one of the possible states of an NXT robot.
data RobotState = RobotState { push_1  :: Bool
                             , push_2  :: Bool
                             , light   :: Bool
                             , optical :: (Word8,Word8,Word8)
                             , motor_1 :: Int
                             , motor_2 :: Int
                             , motor_3 :: Int
                             , r_side  :: R_Side
                             }

-- Data representing the left or right sides.
-- The default will be used for remote control.
data R_Side =  R_Remote | R_Right | R_Left


main :: IO ()
main = do
    startRemoteProgram
    withNXT defaultDevice (r_remote (r_start R_Default)


default_state :: RobotState
default_state = RobotState { push_1  = False
                               , push_2  = False
                               , light   = False
                               , optical = (0,0,0)
                               , motor_1 = 0
                               , motor_2 = 0
                               , motor_3 = 0
                               , r_side  = R_Remote
                               }

r_start :: R_Side -> RobotState
r_start R_Default = default_state
r_start R_Right   = default_state { r_side = R_Right }
r_start R_Left    = default_state { r_side = R_Left  }



r_remote :: RobotState -> NXT RobotState
r_remote st = do
    f <- getChar >>= parse_char
  where
      parse_char c st
          | 'q' = sendRemoteCommand (RemoteCommand [A] (SetTo (motor_1 st + 10) 100))
                  >> st { motor_1 + 10 }
