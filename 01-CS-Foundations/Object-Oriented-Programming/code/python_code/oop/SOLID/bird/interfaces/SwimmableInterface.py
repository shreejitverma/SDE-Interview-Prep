# Author: Shreejit Verma
 # GitHub: https://github.com/shreejitverma

from abc import ABC, abstractmethod

class SwimmableInterface:
    @abstractmethod
    def swim() -> None:
        ...