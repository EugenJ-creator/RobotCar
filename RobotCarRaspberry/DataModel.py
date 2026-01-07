from dataclasses import dataclass
from enum import Enum
import json

class DataModelType(Enum):
    RTC_OFFER = "RTC.Offer"
    RTC_ICE_CANDIDATE = "RTC.IceCandidate"
    RTC_ANSWER = "RTC.Answer"
    RTC_START_CALL = "RTC.StartCall"
    RTC_END_CALL = "RTC.EndCall"
    CONTROL_STEAR_CAMERA = "Control.StearCamera"
    RADAR_COORDINATES = "Radar.Coordinates"



@dataclass
class DataModel:
    

    target: str
    sender: str
    data: str | dict
    type: DataModelType

    # def __init__(self, target, sender, data, type):
    #         target = target
    #         sender = sender
    #         data = data
    #         type = type


    @property
    def target(self):
        return self._target

    @target.setter
    def target(self, value):
        self._target = value

    @property
    def sender(self):
        return self._sender

    @sender.setter
    def sender(self, value):
        self._sender = value

    @property
    def data(self):
        return self._data

    @data.setter
    def data(self, value):
        self._data = value

    @property
    def type(self):
        return self._type

    @type.setter
    def type(self, value):
        self._type = value

    
    
    def __post_init__(self):
        if not isinstance(self.type, DataModelType):
            raise TypeError("type must be DataModelType")
        
    def to_dict(self) -> dict:
        return {
            "target": self._target,
            "sender": self._sender,
            "data": self._data,
            "type": self._type.value  # ✅ enum → string
    }


# --- Function to deserialize JSON to DataModel ---
def json_to_datamodel(json_str: str) -> DataModel:
    json_obj = json.loads(json_str)
    get_or_null = lambda obj, key, default=None: obj.get(key, default)
    
    return DataModel(
        # target=json_obj['target'],
        # sender=json_obj['sender'],
        # data=json_obj['data'],
        # type=DataModelType(json_obj['type'])  # Convert string to Enum



        target=get_or_null(json_obj, 'target'),
        sender=get_or_null(json_obj, 'sender'),
        data=get_or_null(json_obj, 'data'),
        type=DataModelType(json_obj['type'])


    )

