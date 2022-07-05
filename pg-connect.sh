PODNAME=$(kubectl get pods -l app=postgres -o name)
kubectl exec -it $PODNAME -- psql -h localhost -U gameservice --password -p 5432 gameservice
