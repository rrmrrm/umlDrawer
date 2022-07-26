from array import array
from asyncio.windows_events import NULL
from math import fabs
import sys
import os
from unicodedata import name

from tensorflow.python.keras import regularizers
from tensorflow.python.keras.layers.core import Dropout

import pandas as pd
import numpy as np
import tensorflow as tf
from tensorflow.keras import layers
from tensorflow.keras.layers.experimental import preprocessing
# ujratanítás csak tf.keras.Model-el működik, sequential-al nem.


#inputFileName = 'trainingData.csv'
#outputFileName = 'trainedNetwork.txt'

# háló betöltése és áttanítása:
retrain = True
# áttanításkor a betöltött súlyok változatlanok maradjanak-e:
freezeKeptWights = True
#retrain = False
saveModelPath = 'newSavedTfModel'
loadModelPath = 'savedTfModel'

#a tanítómintaként használt képek pixeleinek száma:
inputSize = 400
expOutputSize = 6
epochs_num = 300
#az adatsor pár eleme test mintaként lesz használva, a testDataRation arányban:
testDataRatio = 0.6
batchSize= 2
outputFileName = "trainedNetwork.txt"

train = pd.DataFrame()

inputFileName = "concatenatedCsv/concatenated.csv"
np.set_printoptions(precision=5, suppress=True)
names_array = np.empty(0)
print(names_array)
for i in range(0, inputSize):
    names_array = np.append(names_array, "cell_"+str(i))
for i in range(0, expOutputSize):
    names_array = np.append(names_array, "out_"+str(i))
#print(names_array)
#a names_array arra kell, hogy a train nevű DataFramenek legyenek oszlopnevei
train = pd.read_csv(
    inputFileName, header=None, names=names_array)

print(train.head())
features = train.copy()

#megkeverem az adatokat(ez a tanítást segíti, és enélkül a test adatok tömbjébe nem feltétlen jutna mindegyik osztályhoz-hoz tartozó mintából):
#forrás: https://stackoverflow.com/questions/29576430/shuffle-dataframe-rows
print("shuffling data")
features = features.sample(frac=1).reset_index(drop=True)

print("features after shuffling:")
print(features)
#labels csak az elvárt outputokat fogja tartalmazni, features pedig a bemeneteket:
labels = []
#a labels-be kerülnek a features-ből az elvárt kimenetekhez tartozó oszlopok:
for i in range(0, expOutputSize):
    outputCol = features.pop("out_"+str(i))
    labels.append(outputCol)
labels = np.array(labels)
#az append függvény a feature oszlopait töltötte bele a labels-be, ezért a labels-t transzponálni kell:
labels = np.transpose(labels)
features = np.array(features)

print("features after label separation:")
print(features)
print("labels after separation:")
print(labels)

#kettébontom az adathalmazt teszt és tanító mintákra testDataRation arányban:
sliceAtTrain = int( len(features)*testDataRatio )
featuresTrain = []
labelsTrain = []
featuresTest= []
labelsTest = []
#sliceAtTrain = 0 külön eset, mert tomb[:-0] ures tombot jelent:
if sliceAtTrain == 0:
    featuresTrain = features
    labelsTrain = labels
    featuresTest= []
    labelsTest = []
else:
    featuresTrain = features[ : -sliceAtTrain]
    labelsTrain = labels[ : -sliceAtTrain ]
    featuresTest= features[-sliceAtTrain : ]
    labelsTest = labels[-sliceAtTrain : ]

print("trainingData size:", len(featuresTrain))
print("testData size:", len(featuresTest))
model = NULL
if retrain:
    print("loading model from " + loadModelPath)
    modeltoRetrain = tf.keras.models.load_model(loadModelPath)
    
    print("training accuracy of loaded model before retraining:")
    print(modeltoRetrain.evaluate(featuresTrain, labelsTrain, batch_size=1))
    print("validation accuracy of loaded model before retraining:")
    print(modeltoRetrain.evaluate(featuresTest, labelsTest, batch_size=1))

    layersList = [l for l in modeltoRetrain.layers]

    #hány réteget tartsunk meg a betöltött hálóból(0nál nagyobbnak kell lennie):
    layersToKeep = len(layersList) - 1
    assert(layersToKeep > 0)
    
    # layerSizesToReplaceRest: az új létrehozandó rétegek méretei.
    # Az utolsó elem felül lesz írva, expOutputSize-ra, így az nem fogja meghatározni semelyik réteg méretét,
    #  de layerSizesToReplaceRest minden egyes eleme egy új réteget jelent
    layerSizesToReplaceRest = [20, 2]
    
    # opcionális: a meghagyott rétegek tanítását megakadályozom:
    if freezeKeptWights:
        print("freezing loaded weights before retraining.")
        for i in range(0, layersToKeep):
            layersList[i].trainable = False
    else:
        print("un-freezing loaded weights before retraining.")
        for i in range(0, layersToKeep):
            layersList[i].trainable = True
    print("network structure before reaplcing/adding new layers:")
    modeltoRetrain.summary()

    # a betöltött háló rétegeinek méretét egy listában tárolom, az elemei határozzák majd meg az új rétegek számát és méretét:
    layersOutputShapes = [l.output_shape for l in layersList]
    #az inputlayer speciális:
    layersOutputSizes = [inputSize]
    # a többi réteg méretét is megadjuk:
    for i in range(1, layersToKeep):
        layersOutputSizes.append(layersOutputShapes[i][1])
    print("a meghagyott rétegek méretei: ")
    print(layersOutputSizes)

    # itt adom hozzá layersOutputSizes-hez az új létrehozandó rétegek méretét.
    # Az utolsó elem felül lesz írva így az nem fogja meghatározni semelyik réteg méretét,
    #  de minden egyes hozzáadott elem egy új réteget jelent.(ld. lejjebb)
    for s in layerSizesToReplaceRest:
        layersOutputSizes.append(s)

    # az utolsó réteg perceptronjainak a számát frissítjük, hogy a háló képes legyen a megfelelő számú alakzat megkülönböztetésére:
    layersOutputSizes[len(layersOutputSizes)-1] = expOutputSize
    print("new layersOutputSizes: ")
    print(layersOutputSizes)
    # az utolsó meghagyott réteg output-ját átadjuk az első új rétegnek:
    x = modeltoRetrain.layers[layersToKeep-1].output
    for i in range(layersToKeep, len(layersOutputSizes)):
        print(layersOutputSizes[i])
        #x = layers.Dropout(0.1, name="dropout_"+str(i)+"new")(x)
        x = layers.Dense(units=layersOutputSizes[i], activity_regularizer=regularizers.L2(0.01), activation='sigmoid', name="dense_"+str(i)+"replaced")(x)

    result_model = tf.keras.Model(inputs=layersList[0].input, outputs=x)

    print("new network structure:")
    result_model.summary()
    
    model = result_model
else:
    print("creating model")
    inputs = tf.keras.Input(inputSize)
    x = inputs
    x = layers.Dense( 60, activation='sigmoid', activity_regularizer=regularizers.l1_l2(0.007,0.007) )(x)
    #x = layers.Dense(20, activation='sigmoid')(x)
    outputs = layers.Dense(expOutputSize, activation='sigmoid')(x)
    model = tf.keras.Model(inputs=inputs, outputs=outputs)
    
print("compiling model")

# CCE = CategoricalCrossentropy
# from_logits=True: azt jelezzük ezzel, hogy a az utolsó nem softmax() réteg, így a kimenet 'raw logit' okat tartalmaz. Ez információ alapján a CCE loss függvény először a softmax() függvényt végrehajtja a predikciókon, és csak ezután számol hibát.
lossFunc = tf.keras.losses.CategoricalCrossentropy(from_logits=True)
metricsFunc = tf.keras.metrics.CategoricalAccuracy()
#metricsFunc = tf.keras.metrics.MeanSquaredError()
#lossFunc = tf.losses.MeanSquaredError()

# ha új modelt hoztunk létre, akkor IS comppile-olni kell,
#  de akkor is ha betöltöttünk egy betan\tottat, mert csak a model.compile hívás hatására érvényesül a layer[i].Trainable=False és asszem az új rétegek hozzáadaása is
model.compile( 
    optimizer = tf.optimizers.Adam(
        learning_rate=0.0001#0.001
    ),
    loss = lossFunc, 
    metrics = metricsFunc
)
model.summary()

#tanítás:
print("training model:")
early_stopping = tf.keras.callbacks.EarlyStopping(
    monitor='val_loss',
    patience = 220,
    mode = 'min',
    restore_best_weights=True
)
def fit(m):
    #ha a test halmaz ures, akkor nem hasznaljuk validaciora:
    if len(featuresTest) == 0:
        history = m.fit(
            featuresTrain, 
            labelsTrain,
            batch_size=batchSize, 
            epochs=epochs_num,
            callbacks = [early_stopping]
        )
    else:
        history = m.fit(
            featuresTrain, 
            labelsTrain,
            validation_data=(featuresTest, labelsTest),
            batch_size=batchSize, 
            epochs=epochs_num,
            callbacks = [early_stopping]
        )
    return history

print("modell tanítása:")
print(str(fit(model)))

#tesztelés:
print("testing model:")
testResult = [0.0, 0.0]

#ha nincs test data, akkor a training data-ra vonatkozó pontosságát vetjük össze a modelleknek:
if len(featuresTest) == 0:
    print("no validation(test) data. comparing training accuracy of model")
    testResult = model.evaluate(featuresTrain, labelsTrain, batch_size=1)
    print("also evaluating model with batchsize used for training(just for show)(loss and metrics):")
    print(model.evaluate(featuresTrain, labelsTrain, batch_size=batchSize))
else:
    print("comparing validation accuracy of model:")
    testResult = model.evaluate(featuresTest, labelsTest, batch_size=1)
    print("also evaluating model with batchsize used for training(just for show)(loss and metrics):")
    print(model.evaluate(featuresTest, labelsTest, batch_size=batchSize))

print("test results(loss and metrics):", testResult)

###---------------------------------------------------------------------------------------------------------------------------------------------
#ezután konvertálom át a modellbeli neurális hálót olyan formátumúra, hogy a c++ programom használhassa majd azt:
# a c++ program ilyen formában várja a neurális hálót tartalmazó file-t:
#minden rétegre(kimeneti réteget is beleszámítva): 
#   <réteg mérete>+1, placeholder , {réteg elemeinek sorozata}
#minden élsúly mátrixra: 
#   <bal réteg mérete>+1,
#   <jobb réteg mérete>+1,
#   placeholder, {bias értékek sorozata},
#   a mátrix minden oszlopára:
#       <jobb réteg mérete>+1>, placeholder, {élsúlyok sorozata}
###---------------------------------------------------------------------------------------------------------------------------------------------

print("saving most accurate model to " + saveModelPath)
model.save(saveModelPath)

print("openin "+outputFileName)
netOut = open(outputFileName,"w")
print("saving most accurate model to " +outputFileName)
#ha integrateBias 1, akkor a bias vektorok a súlymátrixok első oszlopába lesznek ágyazva, 
# és a mátrixok első sora csupa 1essel lesz kiegészítve,
# így a mátrixok szélessége és magassága 1-el megnő
integrateBias = 1

#kiírjuk a perceptron rétegek struktúráját először(csupa 1-el kitöltve a perceptronok outputjuait tároló tömböket).
#a programomban a kimeneti réteget is beleszámolom a rétegek közé így 1-el több réteg kell hogy legyen, mint élmátrix
#nem-dropout retegek megszamolasa:
lNum = 0
for lay_ in model.layers:
    if 'dropout' not in lay_.name:
        lNum += 1

assert(not isinstance(model, tf.keras.Sequential))
netOut.write(  str( lNum) + "\n"  )
for l in range(1, len(model.layers) + 1):
    #a dropout layereket átugorjuk:
    # Check if a Layer is an Input Layer using its name
    if l < len(model.layers) and 'dropout' in model.layers[l].name:
        continue;
    #az utolsó iteráció kivételes, mert az uccsó iterációban az l. réteg nem létezik
    if l == len(model.layers):
        mx = model.layers[l-1].get_weights()[0]
        # map(aktivacios_func, mx*bemenet + Vbias) = kimenet
        # a bemeneti réteg mérete: len(mx)
        # a kimeneti réteg mérete: len(mx[0]) 
        if len(mx) > 0:
            netOut.write(str( len(mx[0]) + integrateBias ) + "\n")
        #ha integrate bias 1, akkor a perceptron rétegek 1-el több elemet tartalmaznak(de a 0. elem nem igazi perceptron, csak a bias értékek foglalja a helyet)
        if integrateBias == 1:
            netOut.write("1 ")
        #a programomban a kimeneti réteget is beleszámolom a rétegek közé így 1-el több réteg kell hogy legyen, mint élmátrix,
        #és ezen réteg mérete az utolsó élmátrix második dimenziójával egyezik meg(a mátrix sorainak számával):
        if len(mx) > 0 and len(mx[0]) > 0:
            for cnt in mx[0]:
                netOut.write("0 ")
            netOut.write("\n")

    else:
        mx = model.layers[l].get_weights()[0]
        #ha integrate bias 1, akkor a perceptron rétegek 1-el több elemet tartalmaznak(de a 0. elem nem igazi perceptron, csak a bias értékek miatt van)
        netOut.write(  str( len(mx) + integrateBias) + "\n"  )
        if integrateBias == 1:
            netOut.write("1 ")
        #a súlymátrix 1. dimenziója, azaz az oszlopok száma adja meg a hozzá tartozó réteg elemszámát:
        for col in mx:
            netOut.write("0 ")
        netOut.write("\n")
netOut.write("\n")

#a súlymátrixok  kiírása következik: 
netOut.write(  str( lNum-1 ) + "\n"  )
for l in model.layers[1:]:
    #a dropout layereket átugorjuk:
    # Check if a Layer is an Input Layer using its name
    if 'dropout' in l.name:
        continue;

    biases = l.get_weights()[1]
    matrix = l.get_weights()[0]
    if len(matrix) == 0:
        continue
    #a mátrix oszlopainak száma:
    netOut.write(    str(  len(matrix) + integrateBias  ) + "\n"   )
    #ha integrateBias 1, akkor a mátrix első oszlopa a bias tömb lesz:
    if integrateBias == 1:
        #a mátrix sorainak száma:
        netOut.write(  str( len(matrix[0]) + integrateBias ) + "\n" )
        netOut.write("1 ")
        for bias in biases:
            netOut.write( str(bias) + " " )
        netOut.write("\n")
    for v in l.get_weights()[0]:
        #v az l réteghez tartozó súlymátrix oszlopain iterál
        netOut.write(  str( len(v) + integrateBias ) + "\n" )
        if integrateBias == 1:
            netOut.write("1 ")
        for weight in v:
            #w az l réteg egy súlymmátrixának v oszlopának elemein iterál, tehát a mátrix egy oszlopa a file egy sorának fog megfelelni
            netOut.write( str(weight) + " ")
        netOut.write("\n")
    #ha integrateBias 0, akkor l réteg perceptronjainak bias vektorát külön írjuk ki:
    if integrateBias == 0:
        netOut.write("\n")
        netOut.write(    str(  len( l.get_weights()[1] )  )    )
        for bias in biases:
            netOut.write(str(bias) + " ")
    netOut.write("\n")
netOut.close()
###---------------------------------------------------------------------------------------------------------------------------------------------

sys.stdout.close()
sys.stderr.close()