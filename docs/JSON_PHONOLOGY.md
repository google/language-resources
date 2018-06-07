## JSON Phonology V1

JSON phonology has four fields which are **name**, **feature_types**, **features** and **phones**

Example JSON phonlogy can be found [here](https://github.com/googlei18n/language-resources/blob/master/si/festvox/ipa_phonology.json)

**Name** - This contains the name of the phoneme inventory .

**Feature Types** - This is an array of arrays. It contains different classes of phonemes. Every sub-array corresponds to a different phoneme class. The first element of each sub-array is the class name and the remaining elements are property types associated with that class.

**Example** -
```
 "feature_types" : [
       ["consonant","voicing", "place", "manner"],
       ["vowel", "rounding", "height", "frontness", "length"],
       ["silence"]
   ],
```
In the first sub-array, consonant is the class name. This class has 3 possible property types “voicing”, “place” and “manner”.

**Features** - This contains possible values for the above mentioned property types. This is an array of arrays. The first element of the first sub-array is the name, due to festival compatibility reasons this is typically set to “vc” (vowel or consonant), the remaining elements contains names of the phoneme classes. The rest of the sub-arrays have the following format: the first element is a property type name (which appears in "feature_types" array), the remaining elements are possible values for that property type.

**Example** - 
```
"features": [
	 ["vc", "consonant", "vowel", "silence"],
	 ["voicing", "voiced", "voiceless"],
   …
]
```
The first element of the first sub-array is the name of the properties “vc” and the remaining elements are the phoneme classes.


The second sub-array is ‘["voicing", "voiced", "voiceless"]’. In this sub-array, the first element “voicing” is a property of the phoneme class “vowel”, which can be found in the feature_types" array. The remaining elements “voiced” and “voiceless” refers to the possible values for the property “voicing”.



**Phones** - This is an array of arrays where the first element of each sub-array is the phoneme symbol. The second element is the class name of that phoneme (refers to "feature_types"). The remaining elements are property values required for a phoneme in that class.

**Example** -

```
 "phones": [
      ["p",  "consonant", "voiceless", "labial", "stop"],
      .....
  ]
```

The first sub-array is the phone “p” (indicated by the first element) which is a consonant (indicated by the second element). Its “voicing” property value is “voiceless”. Its “place” property value (place of articulation) is “labial” and its “manner” (manner of articulation) is “stop”.



