import yaml

with open("tacot.yaml", 'r') as stream:
    data = yaml.load(stream)

print(data['name'])
print(data['density']['virgin'])
print(data['specific_heat']['virgin']['polynomial']['coefficients'])
print(data['decomposition_model']['A'])
